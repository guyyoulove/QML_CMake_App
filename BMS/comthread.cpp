#include "comthread.h"
#include <QDebug>
#include <QDateTime>
#include <QTimer>
ComThread::ComThread(QObject *parent):QThread(parent),m_quit(false),m_baudRate(0),m_dataBits(0),
    m_stopBits(0),m_waitTimeOut(30000),m_isCycle(false),m_pause(false),
    m_newData(false),m_comFlag(0),m_batteryCnt(0),
    m_waitRx_MS(50),m_waitRx_byte_MS(30),m_rxFailWait_MS(200),m_rxWait_MS(1000),
    m_comFailCnt(20),m_continueCom(false),m_comSleep(false)
{

    connect(&timer, SIGNAL(timeout()), this, SLOT(tenSecShot()));

}

ComThread::~ComThread()
{
    {
        QMutexLocker locker(&m_mutex);
        m_quit=true;
        m_pause=true; //如果正在执行读则暂停
        m_cond.wakeOne();
    }
    wait();
}

void ComThread::transaction(const QString &portName,const uint &baudRate,const int& dataBits,
                            const int &stopBits,const QString &parity,QList<QByteArray>& commandList,bool isCycle,quint8 comFlag)
{
    //qDebug()<<"ComThreadFtransaction";
    //qDebug()<<commandList.size();
    for(int i=0;i<commandList.size();i++){
        //qDebug()<<commandList[i].toHex();
    }
    m_commandList.clear();
    //主线程调用该函数
    //qDebug()<<portName<<baudRate<<dataBits<<stopBits<<parity;
    QMutexLocker locker(&m_mutex);
    m_portName=portName;
    m_baudRate=baudRate;
    m_dataBits=dataBits;
    m_stopBits=stopBits;
    m_quit=false;
    m_pause=false;
    m_commandList=commandList;

    if(m_commandList.empty()){  //列表中如果没有数据，则新数据标识为假
        m_newData=false;
    }
    else {
        m_newData=true;
    }

    m_isCycle=isCycle;
    m_comFlag=comFlag;

    if("None"==parity)
        m_parity=QSerialPort::NoParity;
    else if("Even"==parity)
        m_parity=QSerialPort::EvenParity;
    else if("Odd"==parity)
        m_parity=QSerialPort::OddParity;

    //    qDebug()<<m_commandList.size();
    //    for(int i=0;i<m_commandList.size();i++){
    //        qDebug()<<m_commandList[i].toHex();
    //    }
    m_comSleep=false;
    wakeUp();

    if(!isRunning())
        start();
    else
        m_cond.wakeOne();

}

void ComThread::closeSerialPort()
{
    //qDebug()<<"ComThread::closeSerialPort";
    QMutexLocker locker(&m_mutex);
    m_quit=true;
    m_pause=true; //如果正在执行读则暂停
    m_cond.wakeOne();
}


void ComThread::run()
{


    //qDebug()<<"--------通讯线程开始-------------";
    bool currentPortChanged=true;

    m_mutex.lock();//锁上
    QString currentPortName=m_portName;
    uint currentBaudRate=m_baudRate;
    QSerialPort::DataBits currentDataBits=static_cast<QSerialPort::DataBits>(m_dataBits);
    QSerialPort::StopBits currentStopBits=static_cast<QSerialPort::StopBits>(m_stopBits);
    QSerialPort::Parity currentParity=m_parity;
    uint currentWaitTimeOut=m_waitTimeOut;
    m_mutex.unlock(); //开锁

    bool currentPause=false; //暂停，暂停时不发送数据
    bool currentCycle=false; //本次发送的数据帧列表是否循环发送
    quint8 currentComFlag=0;
    quint8 currentBatteryCnt=0;
    quint16 currentWaitRx_MS=0;
    quint16 currentWaitRx_byte_MS=0;
    quint16 currenRxFailWait_MS=0;
    quint16 currenRxWait_MS=0;
    quint32 currentComFailCnt=0; //记录通讯连续失败了多少次
    bool currentContinueCom;
    QList<QByteArray> currentCommandList;//本次发送的数据帧列表

    //qDebug()<<"--------通讯参数-------------";
    //qDebug()<<"portName "<<currentPortName;
    //qDebug()<<"baudRate "<<currentBaudRate;
    //qDebug()<<"dataBits "<<currentDataBits;
    //qDebug()<<"stopBits "<<currentStopBits;
    //qDebug()<<"parity "<<currentParity;

    QSerialPort serial;

    serial.setPortName(currentPortName);
    serial.setBaudRate(currentBaudRate);
    serial.setDataBits(currentDataBits);
    serial.setStopBits(currentStopBits);
    serial.setParity(currentParity);
    if(!serial.open(QIODevice::ReadWrite)){
        //打开串口失败，发送打开串口失败的信号以提示用户，
        //并且退出此线程
        emit responseMsg(tr("打开串口%1失败,错误代码%2")
                         .arg(currentPortName).arg(serial.error()),true);

        return;
    }
    else{
        //打开串口成功
        emit responseMsg(tr("打开串口成功"),false);
    }

    serial.clear(QSerialPort::AllDirections);

    while(!m_quit){
        //qDebug()<<"--------线程开始-----------";
        m_mutex.lock();//锁上
        currentCycle=m_isCycle;
        currentCommandList.clear();
        currentCommandList=m_commandList;
        currentComFlag=m_comFlag;
        currentBatteryCnt=m_batteryCnt;
        m_newData=false;
        m_mutex.unlock(); //开锁

        //-------------开始发送数据----------------

        quint16 index=0;  //发送数据帧列表的索引值
        while( !m_pause && (index<currentCommandList.size())){

            m_mutex.lock();//锁上
            currentWaitRx_MS=m_waitRx_MS;
            currentWaitRx_byte_MS=m_waitRx_byte_MS;
            currenRxFailWait_MS=m_rxFailWait_MS;
            currenRxWait_MS=m_rxWait_MS;

            m_mutex.unlock(); //开锁

            QByteArray txData=currentCommandList.at(index);

            //计算校验，除帧头外相加取反，取低十六位，高低字节交换
            QByteArray txResult=calCheck(txData);//requestData包含帧头5a a5
            txData.append(txResult);

            bool rxSuccess=false;  //接收成功标志
            uint sendCnt=0;//同一帧发送的次数，没有回复的情况下最多重复发一次
            /*-----------------------------------------*/
            while(!rxSuccess && !m_pause){  //未接收到回复的情况下(rxSuccess==false),则再重复发送两次
                //qDebug()<<"发送"<<txData;
                qDebug()<<"发送"<<txData.toHex();
                QString rxTime;
                serial.write(txData);
                if(serial.waitForBytesWritten(500)){
                    //发送成功
                    if(!m_comSleep){  //如果不是在m_comSleep的情况下发送数据帧，则判断回复
                        if(serial.waitForReadyRead(currentWaitRx_MS)){ //等待回复的延时  ？？
                            //有回复
                            //qDebug()<<"serial.bytesAvailable()"<<serial.bytesAvailable();
                            //rxTime.append(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")).append("\r\n");
                            //qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
                            QByteArray rxData=serial.readAll(); //data接收到的回复的数据

                            while(serial.waitForReadyRead(currentWaitRx_byte_MS)){ // 字节间间隔？？
                                //rxTime.append(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")).append("\r\n");
                                //qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
                                rxData+=serial.readAll();
                            }

                            quint16 rxDataLength=0;

                            //检查rxData是否正确
                            //rxDataLength 正确应答应该返回的数据帧长度（即返回的字节数）
                            if(0==currentComFlag){
                                rxDataLength=43;
                            }
                            else if(1==currentComFlag && 0==index){
                                rxDataLength=15;
                            }
                            else if(1==currentComFlag && 1==index){
                                if(currentBatteryCnt>=23)
                                    rxDataLength=119;
                                else {
                                    rxDataLength=49+2*currentBatteryCnt;
                                }
                            }
                            else if(1==currentComFlag && 2==index){
                                rxDataLength=15;
                            }
                            else if(currentComFlag>=2 && currentComFlag<=9){ //写参数的回复
                                rxDataLength=9;
                            }
                            else if(10==currentComFlag){ //读参数的回复
                                rxDataLength=209;
                            }
                            else if(currentComFlag>=11 && currentComFlag<=18){  //温度校准的回复
                                rxDataLength=14;
                            }
                            else if(19==currentComFlag || 20==currentComFlag){  //电流校准的回复
                                rxDataLength=14;
                            }
                            else if(21==currentComFlag){ //读no141的回复
                                rxDataLength=11;
                            }
                            else if(22==currentComFlag){ //读no101_115的回复
                                rxDataLength=39;
                            }
                            else if(23==currentComFlag){ //读rtcTime的回复
                                rxDataLength=16;
                            }
                            else if(24==currentComFlag){ //读logCnt的回复
                                rxDataLength=14;
                            }
                            else if(25==currentComFlag){ //返回日志信息
                                rxDataLength=32;
                            }
                            else if(26==currentComFlag){ //删除ROM日志的回复
                                rxDataLength=13;
                            }
                            else if(27==currentComFlag){ //写Afe数据
                                rxDataLength=9;
                            }
                            else if(28==currentComFlag){ //读Afe数据
                                rxDataLength=37;
                            }
                            else if(29==currentComFlag){ //校准页读实时温度和实时总电流,先读温度，再读电流值
                                if(0==index)
                                    rxDataLength=25;
                                else
                                    rxDataLength=11;
                            }
                            else if(30==currentComFlag){ //写参数no116-118
                                rxDataLength=9;
                            }
                            else if(31==currentComFlag){ //电池信息页校准控制命令
                                rxDataLength=14;
                            }
                            else if(32==currentComFlag){ //电池信息页mos命令
                                rxDataLength=14;
                            }
                            //1、检查校验码是否正确
                            //qDebug()<<"rxDataLength"<<rxDataLength;
                            if(0x00==rxData.at(rxData.size()-1) && rxData.size()==(rxDataLength+1)){
                                //最后一个是00,去掉00
                                rxData=rxData.remove(rxData.size()-1,1);
                                //qDebug()<<"去掉00"<<rxData.toHex();
                            }
                            qDebug()<<"rxDataLength"<<rxDataLength;
                            //qDebug()<<"rxDataLength"<<rxData.size();
                            QByteArray rxCheck=rxData.left(rxData.size()-2);//rxCheck等待验证校验码是否正确
                            QByteArray rxCalResult=calCheck(rxCheck); //rxCalResult 计算出的校验码
                            QByteArray rxResult=rxData.right(2); //rxResult 接收到的校验码
                            if(rxResult==rxCalResult){
                                //校验码正确
                                //qDebug()<<"校验码正确";
                                //2、检查返回数据是否为异常应答，检查命令码高四位是否为C，索引为5
                                //qDebug()<<"命令码"<<(quint8)rxData[5];
                                if(192!=(quint8)(rxData[5]&0xc0)){
                                    //没有异常
                                    //3、检查数据帧长度是否正确
                                    if(rxData.size()==rxDataLength){
                                        //数据长度正确，接收到的数据正确，传送数据
                                        rxData.prepend(1,(char)index);//在数据帧前面加上index标识，以判断同一命令列表中返回的哪一条的内容
                                        rxTime.append("接收完毕\r\n");
                                        //qDebug()<<"得到数据";
                                        emit responseData(rxData);
                                        //emit responseTime(rxTime);
                                        rxSuccess=true;
                                        currentComFailCnt=0;  //通讯成功，将失败次数清零
                                    }
                                    else {
                                        //数据帧长度错误
                                        sendCnt++;
                                        emit responseMsg(tr("接收数据帧长度错误！%1").arg(serial.error()),false);
                                    }

                                }
                                else{
                                    sendCnt++;
                                    //数据异常应答，看异常标识  rxData[6]高四位
                                    quint8 errValue=(((quint8)rxData[6]-9)&0xf0)>>4;
                                    if(1==errValue){
                                        //无数据
                                        emit responseMsg("数据异常应答\r\n无数据",false);
                                    }
                                    else if(2==errValue){
                                        //未定义命令
                                        emit responseMsg("数据异常应答\r\n未定义命令",false);
                                    }
                                    else if(4==errValue){
                                        //读写地址错误
                                        emit responseMsg("数据异常应答\r\n读写地址错误",false);
                                    }
                                    else if(8==errValue){
                                        //校验错误
                                        emit responseMsg("数据异常应答\r\n校验错误",false);
                                    }

                                }
                            }
                            else {
                                //校验码错误
                                sendCnt++;
                                emit responseMsg(tr("接收数据校验失败！%1").arg(serial.error()),false);
                            }

                        }
                        else{//未收到回复
                            //没有回复，没有回复的话要重发两次，记录下发送同一帧的次数
                            sendCnt++;
                            emit responseMsg(tr("未收到下位机回复！%1").arg(serial.error()),false);
                        }
                    }
                    else { //m_comSleep 中发送数据帧，无须判断回复
                        if(serial.waitForReadyRead(currentWaitRx_MS)){ //等待回复的延时
                            QByteArray rxDataBlank=serial.readAll(); //data接收到的回复的数据

                            while(serial.waitForReadyRead(currentWaitRx_byte_MS)){ // 字节间间隔
                                rxDataBlank+=serial.readAll();
                            }
                        }
                        rxSuccess=true;
                        serial.clear(QSerialPort::AllDirections);
                    }
                }
                else{
                    //发送失败，通讯失败，发出信号
                    emit responseMsg(tr("通讯失败！发送数据帧失败"),false);
                    //                    m_mutex.lock();
                    //                    m_pause=true;
                    //                    m_mutex.unlock();
                    sendCnt++;
                }

                if(2==sendCnt){
                    //已经发了两次，均告失败，发送下一个数据帧
                    currentComFailCnt++;
                    emit responseMsg(tr("发送两次均无回复！"),false);
                    break;
                }
                else if(1==sendCnt){ //失败了一次，延时100ms，再发
                    currentComFailCnt++;
                    if(!m_quit && !m_pause){
                        msleep(1000);
                    }

                }

            }  //发送一帧数据成功，或者连续发送两次同帧数据失败
            m_mutex.lock();//锁上
            if(currentComFailCnt==m_comFailCnt && !m_continueCom && !m_comSleep){ //达到失败次数上限，并且不继续通讯
                emit responseMsg(tr("通讯失败！没有数据回复或通讯错误"),true);
                currentCommandList.clear();
                m_quit=true;
                m_pause=true;
            }
            m_mutex.unlock(); //开锁

            index++; //索引值加1
            //qDebug()<<"index++"<<index;
            if(currentCommandList.size()==index && currentCycle){  //如果循环发送命令帧列表中的QByteArray
                //qDebug()<<"if";
                index=index%currentCommandList.size();//则又从头开始发
            }
            if(0==index && !m_pause ){  //已经发完一轮，并且没有暂停
                //qDebug()<<"已经发完一轮，并且没有暂停";
                if(rxSuccess){
                    //发送成功
                    //要继续读，且没有暂停，则延时2秒
                    //qDebug()<<"-------延时2s---------";
                    msleep(currenRxWait_MS);//延时2秒    正确通讯时的循环间隔间隔延时 ??
                }
                else {
                    //发送失败
                    msleep(currenRxFailWait_MS);//延时2秒   接收失败间隔延时 ??
                    serial.clear(QSerialPort::AllDirections);
                }

            }
            else if (0!=index && !m_pause) {//还未发完一轮，并且没有暂停
                if(m_comSleep)
                    msleep(50);  //唤醒帧延迟50ms发送一次
            }
        }
        //不需要循环读，睡眠，直到新的发送数据到来


        m_mutex.lock();
        //        if((!m_quit && m_pause) || (currentCommandList.size()==index) ){
        //            qDebug()<<"-------通讯线程睡眠---------";
        //            m_cond.wait(&m_mutex);
        //        }
        if((!m_quit && m_pause) || (!m_quit &&!m_newData) ){ //每隔10s要发唤醒帧，间隔50ms发一次，发十次
            qDebug()<<"-------通讯线程睡眠---------";
            currentCommandList.clear();
            m_comSleep=true;
            emit responseMsg(tr("通讯线程睡眠"),!m_quit && m_pause);  // (!m_quit && m_pause) 即没有退出并且暂停，才判断是否开始读温度或读电池信息
            m_cond.wait(&m_mutex);
        }
        m_mutex.unlock();

        serial.clear(QSerialPort::AllDirections);
    }
    serial.clear(QSerialPort::AllDirections);
    serial.close();
    emit responseMsg(tr("关闭串口成功"),false);
    //qDebug()<<"通讯线程退出";
    m_comSleep=false;
    wakeUp();
}


void ComThread::stopReadInfo()
{
    //qDebug()<<"ComThread::stopReadInfo";
    QMutexLocker locker(&m_mutex);
    m_pause=true;

}



void ComThread::sendChanged(QList<QByteArray> &commandList, bool isCycle,quint8 comFlag,quint8 batteryCnt)
{

    //qDebug()<<"ComThread::sendChanged"<<isCycle;
    QMutexLocker locker(&m_mutex);
    m_newData=true;  //有新的数据来
    m_pause=false;
    m_commandList.clear();
    m_commandList=commandList;
    m_isCycle=isCycle;
    m_comFlag=comFlag;
    m_batteryCnt=batteryCnt;
    m_comSleep=false;
    wakeUp();
    m_cond.wakeOne();//如果线程睡眠，将其唤醒
}

void ComThread::changeDelay(quint16 waitRx_MS, quint16 waitRx_byte_MS, quint16 rxFailWait_MS, quint16 rxWait_MS)
{
    //qDebug()<<"ComThread::changeDelay"<<waitRx_MS<<waitRx_byte_MS<<rxFailWait_MS<<rxWait_MS;
    QMutexLocker locker(&m_mutex);
    m_waitRx_MS=waitRx_MS;
    m_waitRx_byte_MS=waitRx_byte_MS;
    m_rxFailWait_MS=rxFailWait_MS;
    m_rxWait_MS=rxWait_MS;
}

void ComThread::setComFailAction(quint32 &comFailCnt, bool continueCom)
{
    //qDebug()<<"setComFailAction"<<comFailCnt<<continueCom;
    QMutexLocker locker(&m_mutex);
    m_comFailCnt=comFailCnt;
    m_continueCom=continueCom;
}

void ComThread::tenSecShot()
{
    //qDebug()<<"ComThread::tenSecShot()";
    QMutexLocker locker(&m_mutex);
    m_newData=true;  //有新的数据来
    m_pause=false;
    m_commandList.clear();
    for (int i=0;i<10;i++) {
        m_commandList<<QByteArray::fromHex("5aa5");
    }
    m_isCycle=false;
    m_comFlag=30;
    m_batteryCnt=30;
    m_cond.wakeOne();//如果线程睡眠，将其唤醒
}

void ComThread::wakeUp()
{
    if(m_comSleep){
        //qDebug()<<"ComThread::wakeUp timer start";
        timer.start(10000);
    }
    else {
        //qDebug()<<"ComThread::wakeUp timer stop";
        timer.stop();
    }
}

QByteArray ComThread::calCheck(QByteArray &waitCheck)//waitCheck包含帧头5a a5
{
    if(QByteArray::fromHex("5aa5")==waitCheck)
        return "";
    //qDebug()<<"calCheck"<<waitCheck.toHex();
    quint16 sum=0;
    for(int i=2;i<waitCheck.size();i++){
        sum+=(quint8)waitCheck.at(i);
    }
    sum=~sum;
    QByteArray check;
    check=QByteArray::fromHex(check.setNum(sum,16));
    QByteArray result;
    result.resize(2);
    result[0]=check.at(1);
    result[1]=check.at(0);
    return result;
}

