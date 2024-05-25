#include "bms.h"
#include <QDebug>
#include <QList>
#include <algorithm>
#include <cmath>
#include <QFileInfo>
#include <QCoreApplication>
BMS::BMS(QObject *parent) : QObject(parent),m_comFlag(0),m_batteryInfoModel(nullptr),m_batteryCnt(0),m_tempCnt(0),m_currentIndex(0),
    m_logCnt(0),m_readLogCnt(0),m_comFailCnt(20),m_continueCom(false),m_record_Rx_Scale(1),
    m_rx_cnt(0)//,isComSleep(false)

{
    //发送者固定为 52H 接收者固定为31H
    connect(&m_comThread,SIGNAL(responseData(QByteArray)),this,SLOT(getResponseData(QByteArray)));
    connect(&m_comThread,SIGNAL(responseMsg(QString,bool)),this,SLOT(getResponseMsg(QString,bool)));
    connect(&m_comThread,SIGNAL(responseTime(QString)),this,SLOT(getResponseTime(QString)));
    for(int i=0;i<12;i++){
        m_param19_30.append(0);
    }
    for(int i=0;i<2;i++){
        m_param33_34.append(0);
    }
    m_param36.append(0);
    for(int i=0;i<35;i++){
        m_param38_72.append(0);
    }
    m_param38_72[17]=65280;  //55
    m_param38_72[20]=65280;  //58
    m_param38_72[23]=65280;  //61
    m_param38_72[26]=65280;  //64

    for(int i=0;i<3;i++){
        m_param80_82.append(0);
    }
    m_param86.append(0);
    m_param90.append(0);
    for(int i=0;i<6;i++){
        m_param94_99.append(0);
    }

    for(int i=0;i<3;i++){
        m_param116_118.append(0);
    }

    m_paramPre<<QByteArray::fromHex("5aa500073d821c")<<QByteArray::fromHex("5aa500073d822a")<<
                QByteArray::fromHex("5aa500073d822d")<<QByteArray::fromHex("5aa500073d822f")<<
                QByteArray::fromHex("5aa500073d8259")<<QByteArray::fromHex("5aa500073d825f")<<
                QByteArray::fromHex("5aa500073d8263")<<QByteArray::fromHex("5aa500073d8267")<<QByteArray::fromHex("5aa500073d827d");

    for(int i=0;i<28;i++){
        m_afeValue.append(0);
    }

    for (int i=0;i<32;i++) {
        m_recordBatteryCnt.append(4);
    }

    for (int i=0;i<8;i++) {
        m_recordTempCnt.append(4);
    }

    for (int i=0;i<12;i++) {
        m_recordOtherCnt.append(4);
    }

    for (int i=0;i<12;i++) {
        m_record_Other.append(QVariant());

    }

    for (int i=0;i<8;i++) {
        m_correctTempIndex.append(false);
        m_correctTempValue.append(0);
    }



}





void BMS::transaction(const QString &portName, const uint &baudRate, const int &dataBits, const int &stopBits, const QString &parity, const quint8 &index)
{
    //qDebug()<<"BMStransaction";
    m_commandList.clear();
    m_batteryCnt=0;
    m_tempCnt=0;
    //组合数据帧
    m_currentSender=QByteArray::fromHex("3d");
    m_currentReceiver=QByteArray::fromHex("07");
    if(0==index){ //在电池信息界面按下的打开串口
        QByteArray requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("010B2B"));
        m_commandList<<requestData;
        m_comFlag=0;
        m_comThread.transaction(portName,baudRate,dataBits,stopBits,parity,m_commandList,false,m_comFlag);
        m_currentIndex=0;
    }
    else if(1==index){ //在参数配置界面按下打开串口命令
        m_currentIndex=1;
        m_comThread.transaction(portName,baudRate,dataBits,stopBits,parity,m_commandList,false,2);
    }
    else if(2==index){ //校准界面按下打开串口命令
        QByteArray requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("019f19"));
        m_commandList<<requestData;
        requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("01960B"));
        m_commandList<<requestData;
        m_comFlag=29;
        m_currentIndex=2;
        m_comThread.transaction(portName,baudRate,dataBits,stopBits,parity,m_commandList,true,m_comFlag);
    }
    else if(3==index){ //日志
        m_currentIndex=3;
        m_comThread.transaction(portName,baudRate,dataBits,stopBits,parity,m_commandList,false,2);

    }
    else{ //中颖
        m_currentIndex=4;
        m_comThread.transaction(portName,baudRate,dataBits,stopBits,parity,m_commandList,false,2);

    }


}

void BMS::closeSerialPort()
{
    m_comThread.closeSerialPort();

}

void BMS::setBatteryInfoModel(QObject *obj)
{
    m_batteryInfoModel = qobject_cast<BatteryInfoModel*>(obj);

    //-------没有数据时的状态
    //    for(int i=0;i<12;i++)
    //        m_batteryInfoModel->addBatteryInfo("c"+QString::number(i+1),0,false);
}

void BMS::setTempInfoModel(QObject *obj)
{
    m_tempInfoModel=qobject_cast<TempInfoModel*>(obj);
}

void BMS::setLogInfoModel(QObject *obj)
{
    m_logInfoModel=qobject_cast<LogInfoModel*>(obj);
}

void BMS::setOtherInfoModel(QObject *obj)
{
    m_otherInfoModel=qobject_cast<OtherInfoModel*>(obj);
}

void BMS::stopReadInfo()
{
    qDebug()<<"BMS::stopReadInfo";
    m_comThread.stopReadInfo();
}

void BMS::readBatteryInfo(bool includeHead) //includeHead:是否包含最初的no2-18参数 true 为包含 false 为不包含
{
    m_commandList.clear();
    QByteArray requestData;
    bool isCycle=false;
    if(includeHead){ //包含最初的no2-18参数
        requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("010B2B"));
        m_commandList<<requestData;
        m_comFlag=0;
        isCycle=false;
    }
    else { //不包含最初的no2-18参数
        m_commandList<<QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("01620f"));
        if(m_batteryCnt<23){
            m_commandList<<QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("0195")).append(QByteArray::fromHex(QByteArray::number((49+2*m_batteryCnt),16)));
            m_commandList<<QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("01c90f"));
        }
        else{
            m_commandList<<QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("019577"));
        }
        m_comFlag=1;
        isCycle=true;
    }
    m_comThread.sendChanged(m_commandList,isCycle,m_comFlag,m_batteryCnt);
}

void BMS::paramConfig(quint8 comFlag)
{
    //QList<quint16> m_param19_31;
    //写参数，m_comFlag为2

    //    qDebug()<<m_param19_30.size();
    //    for(int i=0;i<m_param19_30.size();i++){
    //        qDebug()<<m_param19_30[i];
    //    }

    QByteArray config;
    if(2==comFlag){ //写参数No19-31
        for(int i=0;i<m_param19_30.size();i++){
            config.append(QByteArray::fromHex(QByteArray::number((quint8)((m_param19_30[i]&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((quint8)(((m_param19_30[i]&0xff00)>>8)+9),16)));

        }
        config.prepend(QByteArray::fromHex("5aa5183d07021c"));
    }
    else if(3==comFlag){ //写参数No33-34
        for(int i=0;i<m_param33_34.size();i++){
            qDebug()<<"写参数No33-34"<<m_param33_34[i];
            config.append(QByteArray::fromHex(QByteArray::number((quint8)((m_param33_34[i]&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((quint8)(((m_param33_34[i]&0xff00)>>8)+9),16)));
            qDebug()<<"写参数No33-34"<<config.toHex();
        }
        config.prepend(QByteArray::fromHex("5aa5043d07022a"));
    }

    else if(4==comFlag){ //写参数No36
        for(int i=0;i<m_param36.size();i++){
            config.append(QByteArray::fromHex(QByteArray::number((quint8)((m_param36[i]&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((quint8)(((m_param36[i]&0xff00)>>8)+9),16)));
        }
        config.prepend(QByteArray::fromHex("5aa5023d07022d"));
    }
    else if(5==comFlag){ //写参数No38-72
        for(int i=0;i<m_param38_72.size();i++){
            config.append(QByteArray::fromHex(QByteArray::number((quint8)((m_param38_72[i]&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((quint8)(((m_param38_72[i]&0xff00)>>8)+9),16)));
        }
        config.prepend(QByteArray::fromHex("5aa5463d07022f"));
    }
    else if(6==comFlag){ //写参数No80-82
        for(int i=0;i<m_param80_82.size();i++){
            config.append(QByteArray::fromHex(QByteArray::number((quint8)((m_param80_82[i]&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((quint8)(((m_param80_82[i]&0xff00)>>8)+9),16)));
        }
        config.prepend(QByteArray::fromHex("5aa5063d070259"));
    }
    else if(7==comFlag){ //写参数No86
        for(int i=0;i<m_param86.size();i++){
            config.append(QByteArray::fromHex(QByteArray::number((quint8)((m_param86[i]&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((quint8)(((m_param86[i]&0xff00)>>8)+9),16)));
        }
        config.prepend(QByteArray::fromHex("5aa5023d07025f"));
    }
    else if(8==comFlag){ //写参数No90
        for(int i=0;i<m_param90.size();i++){
            config.append(QByteArray::fromHex(QByteArray::number((quint8)((m_param90[i]&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((quint8)(((m_param90[i]&0xff00)>>8)+9),16)));
        }
        config.prepend(QByteArray::fromHex("5aa5023d070263"));
    }
    else if(9==comFlag){ //写参数No94_99
        for(int i=0;i<m_param94_99.size();i++){
            config.append(QByteArray::fromHex(QByteArray::number((quint8)((m_param94_99[i]&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((quint8)(((m_param94_99[i]&0xff00)>>8)+9),16)));
        }
        config.prepend(QByteArray::fromHex("5aa50c3d070267"));
    }
    else if(30==comFlag){ //写参数No116_118
        for(int i=0;i<m_param116_118.size();i++){
            config.append(QByteArray::fromHex(QByteArray::number((quint8)((m_param116_118[i]&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((quint8)(((m_param116_118[i]&0xff00)>>8)+9),16)));
        }
        config.prepend(QByteArray::fromHex("5aa5063d07027d"));
        qDebug()<<"写no116-118参数"<<m_param116_118[0]<<m_param116_118[1]<<m_param116_118[2];
    }
    m_commandList.clear();
    m_commandList<<config;
    m_comFlag=comFlag;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);

}

void BMS::readParamConfig()
{
    //读取参数
    m_comFlag=10;
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("011CD1"));
    m_commandList<<requestData;
    m_currentIndex=1;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);
}

/*
paramGindex:参数所在组的index
index:参数所在list视图的index
param:参数值
*/
void BMS::saveParam(const QVariant& param1,const quint8& indexRow,const quint8& indexCol,const QString& viewName)
{
    //得到参数配置页的参数值
    //qDebug()<<"saveParam";
    //qDebug()<<param1<<indexRow<<indexCol<<viewName;
    quint16 param=(quint16)param1.toUInt();
    quint16 param2=(quint16)param1.toInt();
    if("容量参数"==viewName){  //参数位于No19-No30
        if(indexRow==0){   //no19
            m_param19_30[indexRow]=param;
        }
        else if(indexRow==1){ //no90
            m_param90[0]=param;
        }
        else if(indexRow==2){ //no20
            m_param19_30[indexRow-1]=param;
        }
        else if(indexRow>=6 && indexRow<=15){ //no21-30
            m_param19_30[indexRow-4]=param;
        }
        else if(indexRow==3 || indexRow==4){//No33-No34
            m_param33_34[indexRow-3]=param;
        }
        else if(5==indexRow){ //no36
            m_param36[0]=param;
            qDebug()<<"saveParam"<<param;
        }
    }

    else if("充电机参数"==viewName){ //no116-118
        m_param116_118[indexRow]=param;
    }


    //参数位于No38-No72  注意55和几个下拉框
    else if("基本保护"==viewName){
        if(2==indexRow){  //no38-40
            m_param38_72[indexCol]=param;
        }
        else if(3==indexRow){ //no41-43
            m_param38_72[indexCol+3]=param;
        }
        else if(0==indexRow){//no44-46
            m_param38_72[indexCol+6]=param;
        }
        else if(1==indexRow){//no47-49
            m_param38_72[indexCol+9]=param;
        }
        else if(8==indexRow && 0==indexCol){ //no54
            m_param38_72[16]=param;
        }
        else if(8==indexRow && 1==indexCol){ //no55
            m_param38_72[17]=(param&0x3f)|m_param38_72[17];
        }
        else if(9==indexRow && indexCol<2){//no56-57
            m_param38_72[indexCol+18]=param; //18
        }
        else if(6==indexRow && indexCol==0){//no65-66
            qDebug()<<"65"<<param2;
            m_param38_72[27]=param2; //18
        }
        else if(6==indexRow && indexCol==2){//no65-66
            qDebug()<<"66"<<param2;
            m_param38_72[28]=param2; //18
        }
        else if(4==indexRow && indexCol==0){//no67-68
            qDebug()<<"67"<<param2;
            m_param38_72[29]=param2; //18
        }
        else if(4==indexRow && indexCol==2){//no67-68
            qDebug()<<"68"<<param2;
            m_param38_72[30]=param2; //18
        }
        else if(7==indexRow && indexCol==0){//no69-70
            qDebug()<<"69"<<param2;
            m_param38_72[31]=param2; //18
        }
        else if(7==indexRow && indexCol==2){//no69-70
            qDebug()<<"70"<<param2;
            m_param38_72[32]=param2; //18
        }
        else if(5==indexRow && indexCol==0){//no71-72
            qDebug()<<"71"<<param2;
            m_param38_72[33]=param2; //18
        }
        else if(5==indexRow && indexCol==2){//no71-72
            qDebug()<<"72"<<param2;
            m_param38_72[34]=param2; //18
        }
    }
    else if("其他参数"==viewName){
        if((indexRow>=3 && indexRow<=5) && 0==indexCol){//no50-52
            m_param38_72[indexRow+9]=param;
        }
        else if(0==indexRow && 1==indexCol){//no53
            m_param38_72[15]=param;
        }
        else if( (indexRow>=0 && indexRow<=2) && 0==indexCol){ //80-82
            m_param80_82[indexRow]=param;
        }
        else if( (indexRow>=1 && indexRow<=4) && 1==indexCol){ //no94-97
            m_param94_99[indexRow-1]=param;
        }
        else if(6==indexRow){ //no99 no98
            m_param94_99[4+indexCol]=param;
        }
    }
    else if("高级保护"==viewName){
        if(2==indexRow){//no55
            m_param38_72[17]=(m_param38_72[17]&0xff3f)|(param<<6);  //(param<<6)|m_param38_72[17];
        }
        else if(3==indexRow){//no58
            m_param38_72[20]=(m_param38_72[20]&0xff3f)|(param<<6);
        }
        else if(0==indexRow){//no59-60
            m_param38_72[indexCol+21]=param;
        }
        else if(4==indexRow){//no61
            m_param38_72[23]=(m_param38_72[23]&0xff3f)|(param<<6);
        }
        else if(1==indexRow){//no62-63
            m_param38_72[24+indexCol]=param;
        }
        else if(5==indexRow){//no64
            m_param38_72[26]=(m_param38_72[26]&0xff3f)|(param<<6);
        }
    }
    else if("采样电阻值"==viewName){//no86
        m_param86[0]=param;
    }
}

void BMS::exportParam(QString filePath,const QList<QString>& nameInCapModel,const QList<QString>& nameInBasicModel, const QList<QString>&name3InBasicModel,
                      const QList<QString>& nameInAdvanced,const QList<QString>& nameInOther,const QList<QString>& name2InOther)
{
    qDebug()<<"exportParam";
    filePath=filePath.mid(8);
    QFile file(filePath);
    qDebug()<<filePath;
    if (!file.open(QIODevice::WriteOnly)){
        emit responseMsg("请关闭选择的导出文件或者检查文件是否存在！再执行导出操作",true);
        return;
    }
    file.close();
    QXlsx::Document xlsx;
    QString row;
    xlsx.write("A1", "容量参数配置");
    for(int i=0;i<nameInCapModel.size();i++)
    {
        row="A"+QString::number(i+2);
        xlsx.write(row, nameInCapModel[i]);

    }
    xlsx.write("B2", (quint32)m_param19_30[0]*10);
    xlsx.write("B3", (quint32)m_param90[0]*10); //满充容量
    xlsx.write("B4", (double)m_param19_30[1]/100);
    xlsx.write("B5", m_param33_34[0]);
    xlsx.write("B6", m_param33_34[1]);
    xlsx.write("B7", m_param36[0]);
    for(int i=2;i<m_param19_30.size();i++){
        row="B"+QString::number(i+6);
        xlsx.write(row, m_param19_30[i]);
    }

    xlsx.write("E1", "基本保护参数配置");
    for(int i=0;i<nameInBasicModel.size();i++)
    {
        row="E"+QString::number(i+2);
        xlsx.write(row, nameInBasicModel[i]);

    }

    for(int i=0;i<nameInBasicModel.size();i++)
    {
        row="G"+QString::number(i+2);
        xlsx.write(row, "延时");

    }
    for(int i=0;i<name3InBasicModel.size();i++)
    {
        row="I"+QString::number(i+2);
        xlsx.write(row, name3InBasicModel[i]);

    }
    QList<QString> col={ "F", "H", "J","F", "H", "J" };;
    for(int i=0;i<3;i++){
        xlsx.write(col[i]+QString::number(2), m_param38_72[i+6]);
    }
    for(int i=0;i<3;i++){
        xlsx.write(col[i]+QString::number(3), m_param38_72[i+9]);
    }

    xlsx.write(col[0]+QString::number(4), m_param38_72[0]*0.1); //38
    xlsx.write(col[1]+QString::number(4), m_param38_72[1]);  //39
    xlsx.write(col[2]+QString::number(4), m_param38_72[2]*0.1); //40


    xlsx.write(col[0]+QString::number(5), m_param38_72[3]*0.1);//41
    xlsx.write(col[1]+QString::number(5), m_param38_72[4]);//42
    xlsx.write(col[2]+QString::number(5), m_param38_72[5]*0.1);//43

    xlsx.write("F6", (qint16)m_param38_72[29]);
    xlsx.write("J6", (qint16)m_param38_72[30]);
    xlsx.write("F7", (qint16)m_param38_72[33]);
    xlsx.write("J7", (qint16)m_param38_72[34]);
    xlsx.write("F8", (qint16)m_param38_72[27]);
    xlsx.write("J8", (qint16)m_param38_72[28]);
    xlsx.write("F9", (qint16)m_param38_72[31]);
    xlsx.write("J9", (qint16)m_param38_72[32]);
    xlsx.write("F10", m_param38_72[16]*0.1);
    xlsx.write("H10", m_param38_72[17]&0x00ff);
    xlsx.write("F11", m_param38_72[18]);
    xlsx.write("H11", m_param38_72[19]);
    //27-34是负数


    //导出高级保护参数配置
    xlsx.write("A19", "高级保护参数配置");
    for(int i=0;i<nameInAdvanced.size();i++)
    {
        row="A"+QString::number(i+20);
        xlsx.write(row, nameInAdvanced[i]);

    }
    xlsx.write("A26", "采样电阻值");
    xlsx.write("B20", m_param38_72[21]);
    xlsx.write("B21", m_param38_72[24]);
    quint8 i;
    i=(m_param38_72[17]&0xc0)>>6; //["预留","断开充电器","放电恢复"]   ["移除负载","充电解除"]
    qDebug()<<"i"<<i;                       // ["移除负载","充电解除"]   ["移除负载"]
    if(0==i){
        xlsx.write("B22", "预留");
    }
    else if(1==i){
        xlsx.write("B22", "断开充电器");
    }
    else {
        xlsx.write("B22", "放电恢复");
    }
    xlsx.write("B23", (((m_param38_72[20]&0xc0)>>6)==0)?"移除负载":"充电解除");
    xlsx.write("B24", (((m_param38_72[23]&0xc0)>>6)==0)?"移除负载":"充电解除");
    xlsx.write("B25", "移除负载");
    xlsx.write("B26", m_param86[0]*0.1);

    xlsx.write("C20", "延时");
    xlsx.write("C21", "延时");
    xlsx.write("D20", m_param38_72[22]);
    xlsx.write("D21", m_param38_72[25]);


    //导出充电机参数
    xlsx.write("A28", "充电机参数配置");
    xlsx.write("A29", "最高允许充电电压");
    xlsx.write("A30", "最高允许充电温度");
    xlsx.write("A31", "最低允许充电温度");
    xlsx.write("B29", m_param116_118[0]*0.01);
    xlsx.write("B30", m_param116_118[1]);
    xlsx.write("B31", m_param116_118[2]);

    //导出其他参数
    xlsx.write("E19", "其他参数配置");
    for(int i=0;i<nameInOther.size();i++)
    {
        row="E"+QString::number(i+20);
        xlsx.write(row, nameInOther[i]);

    }
    for(int i=0;i<name2InOther.size();i++)
    {
        row="G"+QString::number(i+20);
        xlsx.write(row, name2InOther[i]);

    }
    for(int i=0;i<m_param80_82.size();i++)
    {
        row="F"+QString::number(i+20);
        xlsx.write(row, m_param80_82[i]);

    }
    for(int i=0;i<3;i++)
    {
        row="F"+QString::number(i+23);
        xlsx.write(row, m_param38_72[i+12]);

    }
    xlsx.write("H20", m_param38_72[15]);
    for(int i=0;i<4;i++)
    {
        row="H"+QString::number(i+21);
        xlsx.write(row, m_param94_99[i]);

    }
    xlsx.write("F26", m_param94_99[4]*(-1));
    xlsx.write("H26", m_param94_99[5]);

    xlsx.saveAs(filePath); // save the document as 'Test.xlsx'
    emit exportRecordStatus("导出成功！");
}

void BMS::importParam(QString filePath)
{
    //导入数据时是不用做数据的乘除处理的，因为导出数据时已经做过乘除处理了，表格中存储的数据即是设定的数据
    filePath=filePath.mid(8);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)){
        emit responseMsg("请关闭选择的导入文件或者检查文件是否存在！再执行导入操作",true);
        return;
    }
    file.close();
    qDebug()<<"importParam";
    Document xlsxR(filePath);
    //QList<quint16> paramList;
    if (xlsxR.load()) // 载入excel成功
    {
        int col = 2;
        for(quint8 i=2;i<18;i++){ //导入容量的内容
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                if(4==i)
                    emit paramInCap(var.toDouble(),i-2);
                else
                    emit paramInCap(var.toUInt(),i-2);
            }
            else {
                emit paramInCap(0,i-2);
            }

        }
        col=6;
        for(quint8 i=2;i<12;i++){ //导入基本参数第一列的内容
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                if(i<4)
                    emit paramInBasic1(var.toUInt(),i-2);
                else if(i<6)
                    emit paramInBasic1(var.toDouble(),i-2);

                else if(i<10)
                    emit paramInBasic1(var.toInt(),i-2);

                else if(i<11)
                    emit paramInBasic1(var.toDouble(),i-2);

                else
                    emit paramInBasic1(var.toUInt(),i-2);
            }
            else {
                emit paramInBasic1(0,i-2);
            }
        }
        col=8;
        for(quint8 i=2;i<12;i++){ //导入基本参数第二列的内容
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                if(i<6)
                    emit paramInBasic2(var.toUInt(),i-2);
                else if(i>9)
                    emit paramInBasic2(var.toUInt(),i-2);
            }
            else {
                emit paramInBasic2(0,i-2);
            }
        }
        col=10;
        for(quint8 i=2;i<12;i++){ //导入基本参数第三列的内容
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                if(i<4)
                    emit paramInBasic3(var.toUInt(),i-2);
                else if(i<6)
                    emit paramInBasic3(var.toDouble(),i-2);
                else
                    emit paramInBasic3(var.toInt(),i-2);
            }
            else {
                emit paramInBasic3(0,i-2);
            }
        }

        //-------------------------
        col=2;
        for(quint8 i=20;i<27;i++){ //导入高级保护参数第一列
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                if(i<22)
                    emit paramInAdvanced(var.toUInt(),i-20);
                else if(i==22){
                    if("预留"==var.toString())
                        emit paramInAdvanced(0,i-20);
                    else if("断开充电器"==var.toString())
                        emit paramInAdvanced(1,i-20);
                    else if("放电恢复"==var.toString())
                        emit paramInAdvanced(2,i-20);
                }
                else if(i<26){
                    if("移除负载"==var.toString())
                        emit paramInAdvanced(0,i-20);
                    else if("充电解除"==var.toString())
                        emit paramInAdvanced(1,i-20);
                }
                else
                    emit paramInResistor(var.toDouble(),i-20);
            }
            else {
                emit paramInAdvanced(0,i-20);
            }
        }

        col=4;
        for(quint8 i=20;i<22;i++){ //导入高级保护参数第二列
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                emit paramInAdvanced2(var.toUInt(),i-20);
            }
            else {
                emit paramInAdvanced(0,i-20);
            }
        }

        //-------------------------
        col=6;
        for(quint8 i=20;i<27;i++){ //导入其他参数第一列
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                if(i<26)
                    emit paramInOther(var.toUInt(),i-20);
                else
                    emit paramInOther(var.toInt(),i-20);
            }
            else {
                emit paramInOther(0,i-20);
            }
        }

        //-------------------------
        col=8;
        for(quint8 i=20;i<27;i++){ //导入其他参数第二列
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                if(i<25 || 26==i)
                    emit paramInOther2(var.toUInt(),i-20);
            }
            else {
                emit paramInOther2(0,i-20);
            }
        }
        //-------------导入充电机参数
        col=2;
        for(quint8 i=29;i<32;i++){ //导入其他参数第二列
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                if(29==i)
                    emit paramInCharge(var.toDouble(),i-29);
                else {
                    emit paramInCharge(var.toUInt(),i-29);
                }
            }
            else {
                emit paramInCharge(0,i-29);
            }
        }

    }
    else {
        //载入excel失败，提示
        emit loadExcelFail();
    }
}

void BMS::correct_readTemp()  //m_comFlag  29 校准页读实时温度
{
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("019f19"));
    m_commandList<<requestData;
    requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("01960B"));
    m_commandList<<requestData;
    m_comFlag=29;
    m_currentIndex=2;
    m_comThread.sendChanged(m_commandList,true,m_comFlag,m_batteryCnt);
}

//void BMS::getNo141() //m_comFlag=21
//{
//    m_commandList.clear();
//    QByteArray requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("01960B"));
//    m_commandList<<requestData;
//    m_comFlag=21;
//    m_currentIndex=2;
//    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);

//}


void BMS::set_correctTemp(quint8 index,bool value,qint16 temp)
{
    qDebug()<<"set_correctTemp"<<index<< value <<temp;
    m_correctTempIndex[index]=value;
    m_correctTempValue[index]=temp;
}

void BMS::start_correctTemp()
{
    int i=0;
    for (;i<8;i++) {
        if(m_correctTempIndex[i]){
            //需要校准
            correctTemp(i+1, m_correctTempValue[i]);
            break;
        }
    }

}

QString BMS::correctDate()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd  HH:mm:ss");
}

void BMS::correctTemp(quint8 num, qint16 value)  //num 校准的哪一个温度，从1开始  value 校准的温度值
{
    //温度校准  m_comFlag 从11 到 18  数据域控制命令从7A开始
    qDebug()<<"BMS::correctTemp"<<num<<value;
    m_comFlag=10+num;
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa505").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("06ff0d")).append(QByteArray::fromHex(QByteArray::number(121+num,16))).append(QByteArray::fromHex("0b")).append(QByteArray::fromHex(QByteArray::number((quint8)((value&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((qint8)(((value&0xff00)>>8)+9),16)));
    m_commandList<<requestData;
    m_currentIndex=2;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);
}

void BMS::correctA(quint8 num, qint16 value)//num 校准的哪一个电流，从0开始  value 校准的电流值
{
    //电流校准  m_comFlag 从19 到 20  数据域控制命令为137+num  append(QByteArray::fromHex(QByteArray::number(139+num,16)))
    qDebug()<<"correctA"<<num<<value;
    m_comFlag=19+num;
    m_commandList.clear();
    QByteArray requestData;
    if(0==num){
        //0电流校准   5a a5 03 3d 07 06 ff  0d 8a 09 cs_l cs_h
        requestData=QByteArray::fromHex("5aa503").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("06ff0d8a09"));
    }
    else if (1==num) {
        //工作电流校准  5a a5 05 3d 07 06 ff 0d 8b 0b 值 cs_l cs_h
        requestData=QByteArray::fromHex("5aa505").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("06ff0d8b0b")).append(QByteArray::fromHex(QByteArray::number((quint8)((value&0x00ff)+9),16))).append(QByteArray::fromHex(QByteArray::number((qint8)(((value&0xff00)>>8)+9),16)));
    }
    m_commandList<<requestData;
    m_currentIndex=2;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);

}

void BMS::correctControl(QString value) //电池信息页校准控制命令 m_comFlag 为31
{
    quint8 value_l=(quint8)(value.toUShort(nullptr,16));
    qDebug()<<"correctControl";
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa505").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("06ff0e6b0b")).append(QByteArray::fromHex(QByteArray::number((quint8)((value_l&0x00ff)+9),16))).append(QByteArray::fromHex("ff"));
    m_commandList<<requestData;
    m_comFlag=31;
    m_currentIndex=3;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);

}

void BMS::correctMos(int e_v) //电池信息页mos解除/关闭 m_comFlag 为32
{
    quint8 value_l=e_v;
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa505").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("06ff0e6a0b")).append(QByteArray::fromHex(QByteArray::number((quint8)((value_l&0x00ff)+9),16))).append(QByteArray::fromHex("ff"));
    m_commandList<<requestData;
    m_comFlag=32;
    m_currentIndex=3;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);
}


void BMS::getNo101_115()  //m_comFlag 为22
{
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("016e27"));
    m_commandList<<requestData;
    m_comFlag=22;
    m_currentIndex=3;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);


}

void BMS::getRtcTime()  // m_comFlag 为23
{
    qDebug()<<"BMS::getRtcTime()";
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa503").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("05ff0c8a09"));
    m_commandList<<requestData;
    m_comFlag=23;
    m_currentIndex=3;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);

}

void BMS::getLogCnt() // m_comFlag 为24
{
    qDebug()<<"BMS::getLogCnt";
    m_logInfoModel->clear();
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa503").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("05ff0c8d09"));
    m_commandList<<requestData;
    m_comFlag=24;
    m_currentIndex=3;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);

}

void BMS::getLog() //读日志的m_comFlag 为25
{
    qDebug()<<"BMS::getLog";
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa503").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("05ff0c8f09"));
    for(int i=0;i<m_logCnt;i++){
        m_commandList<<requestData;
    }
    m_comFlag=25;
    m_currentIndex=3;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);
}

void BMS::refresh()
{
    //m_batteryInfoModel->data()
}

void BMS::deleteRomLog()  //清空ROM中日志的m_comFlag 为26
{
    qDebug()<<"BMS::deleteRomLog";
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa503").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("05ff0c8C09"));

    m_commandList<<requestData;

    m_comFlag=26;
    m_currentIndex=3;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);

}

void BMS::changeDelay(quint16 waitRx_MS, quint16 waitRx_byte_MS, quint16 rxFailWait_MS, quint16 rxWait_MS)
{
    m_comThread.changeDelay(waitRx_MS,waitRx_byte_MS,rxFailWait_MS,rxWait_MS);
}

void BMS::saveAfe(int row, int col, qint8 value)
{
    qDebug()<<"saveAfe"<<row<<col<<value;
    m_afeValue[row+col*10]=(quint8)value;
    qDebug()<<"m_afeValue[row+col*10]"<<m_afeValue[row+col*10];
    qDebug()<<"(qint8)m_afeValue[row+col*10]"<<(qint8)m_afeValue[row+col*10];
}

void BMS::saveAfeHex(int row, int col, QString value)
{
    if(value.endsWith('H') || value.endsWith('h')){
        m_afeValue[row+col*10]=(quint8)(value.left(value.size()-1).toUShort(nullptr,16));
    }
    else {
        m_afeValue[row+col*10]=(quint8)(value.toUShort(nullptr,16));
    }
}

void BMS::exportAfe(QString filePath,const QList<QString>& nameInAfeModel,const QList<QString>& name2InAfeModel, const QList<QString>&name3InAfeModel)
{
    filePath=filePath.mid(8);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)){
        emit responseMsg("请关闭选择的导出文件或者检查文件是否存在！再执行导出操作",true);
        return;
    }
    file.close();
    qDebug()<<"exportAfe";
    //如果文件打开，要把文件关闭
    QXlsx::Document xlsx;
    QString row;
    xlsx.write("A1", "AfeROM");
    for(int i=0;i<nameInAfeModel.size();i++)
    {
        row="A"+QString::number(i+2);
        xlsx.write(row, nameInAfeModel[i]);

    }
    for(int i=0;i<name2InAfeModel.size();i++)
    {
        row="C"+QString::number(i+2);
        xlsx.write(row, name2InAfeModel[i]);

    }
    for(int i=0;i<name3InAfeModel.size();i++)
    {
        row="E"+QString::number(i+2);
        xlsx.write(row, name3InAfeModel[i]);

    }
    for (int i=0;i<m_afeValue.size();i++) {
        if(i<2){
            //导出的为hex
            xlsx.write("B"+QString::number(i+2), QString(QString::number((quint8)m_afeValue[i],16)+"H"));
        }
        else if(i<10)
            xlsx.write("B"+QString::number(i+2), (quint8)m_afeValue[i]);
        else if (i<20) {
            if(i>=17)
                xlsx.write("D"+QString::number(i-8), (qint8)m_afeValue[i]);
            else {
                xlsx.write("D"+QString::number(i-8), (quint8)m_afeValue[i]);
            }
        }
        else {  //i>=20
            if(i<=24)
                xlsx.write("F"+QString::number(i-18), (qint8)m_afeValue[i]);
            else if (26==i) { //导出的为hex
                xlsx.write("F"+QString::number(i-18), QString(QString::number((quint8)m_afeValue[i],16)+"H"));
            }
            else {
                xlsx.write("F"+QString::number(i-18), (quint8)m_afeValue[i]);
            }
        }
    }
    xlsx.saveAs(filePath);
    emit exportRecordStatus("导出成功！");
}


void BMS::importAfe(QString filePath)
{
    filePath=filePath.mid(8);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)){
        emit responseMsg("请关闭选择的导入文件或者检查文件是否存在！再执行导入操作",true);
        return;
    }
    file.close();
    qDebug()<<"importAfe";
    Document xlsxR(filePath);
    if (xlsxR.load()) // 载入excel成功
    {
        int col = 2;
        for(quint8 i=2;i<4;i++){  //index 0-1 hex
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                emit valueInAfeHex(i-2,var.toString());
            }
            else {
                emit valueInAfeHex(i-2,"");
            }

        }
        for(quint8 i=4;i<12;i++){  //index 0-9
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                emit valueInAfe(i-2,var.toUInt());
            }
            else {
                emit valueInAfe(i-2,0);
            }

        }
        col = 4;
        for(quint8 i=2;i<9;i++){ //index 10-16
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                emit valueInAfe(i+8,var.toUInt());
            }
            else {
                emit valueInAfe(i+8,0);
            }

        }
        for(quint8 i=9;i<12;i++){ //index 17-19
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                emit valueInAfe(i+8,var.toInt());
            }
            else {
                emit valueInAfe(i+8,0);
            }

        }

        col = 6;
        for(quint8 i=2;i<7;i++){ //index 20-24
            Cell* cell = xlsxR.cellAt(i, col);
            if ( cell != nullptr)
            {
                QVariant var = cell->readValue();
                emit valueInAfe(i+18,var.toInt());
            }
            else {
                emit valueInAfe(i+18,0);
            }

        }

        for(quint8 i=7;i<10;i++){ //index 25-27
            if(8==i){
                //hex
                Cell* cell = xlsxR.cellAt(i, col);
                if ( cell != nullptr)
                {
                    QVariant var = cell->readValue();
                    emit valueInAfeHex(i+18,var.toString());
                }
                else {
                    emit valueInAfeHex(i+18,"");
                }
            }
            else {
                Cell* cell = xlsxR.cellAt(i, col);
                if ( cell != nullptr)
                {
                    QVariant var = cell->readValue();
                    emit valueInAfe(i+18,var.toUInt());
                }
                else {
                    emit valueInAfe(i+18,0);
                }
            }


        }
    }
    else {
        //载入excel失败，提示
        emit loadExcelFail();
    }

}

void BMS::writeAfe()  //m_comFlag 27   QList<quint8> m_afeValue;
{
    qDebug()<<"BMS::writeAfe";
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa51c").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("0409"));

    for(int i=0;i<m_afeValue.size();i++){
        if(i<2){
            requestData.append(QByteArray::fromHex(QByteArray::number((quint8)(m_afeValue[i]+9),16)));
        }

        else if(i<17){
            //u8
            requestData.append(QByteArray::fromHex(QByteArray::number((quint8)(m_afeValue[i]+9),16)));
        }
        else if (i<25) {
            //i8
            requestData.append(QByteArray::fromHex(QByteArray::number((qint8)(m_afeValue[i]+9),16)));
        }
        else {
            //u8
            requestData.append(QByteArray::fromHex(QByteArray::number((quint8)(m_afeValue[i]+9),16)));

        }
    }
    m_commandList<<requestData;

    m_comFlag=27;
    m_currentIndex=3;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);

}

void BMS::readAfe() //m_comFlag 28 读Afe
{
    qDebug()<<"BMS::readAfe";
    m_commandList.clear();
    QByteArray requestData=QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("0309"));
    m_commandList<<requestData;
    m_comFlag=28;
    m_currentIndex=3;
    m_comThread.sendChanged(m_commandList,false,m_comFlag,m_batteryCnt);

}

void BMS::record_Rx_Scale(quint32 scanTime, quint32 recordTime)
{
    qDebug()<<"scanTime recordTime"<<scanTime<<recordTime;
    if(recordTime<scanTime)
        m_record_Rx_Scale=1;
    else {
        m_record_Rx_Scale=recordTime/scanTime;
    }
    qDebug()<<"m_record_Rx_Scale"<<m_record_Rx_Scale;
}




void BMS::recordBatteryInfo(quint8 index, quint16 value,quint8 bal)
{



    m_record_Battery_Index<<index;
    m_record_Battery_Time<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    m_record_Battery_v<<value;
    m_record_Battery_bal<<bal;


    if(1000==m_record_Battery_Index.size()){
        QFile file(QCoreApplication::applicationDirPath()+"/batteryfile.dat");
        qDebug()<<"有1000个数据了";
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QDataStream out(&file);
        for (int i=0;i<m_record_Battery_Index.size();i++) {
            out<<m_record_Battery_Index[i]<<m_record_Battery_Time[i]<<m_record_Battery_v[i]<<m_record_Battery_bal[i];
        }
        m_record_Battery_Index.clear();
        m_record_Battery_Time.clear();
        m_record_Battery_v.clear();
        m_record_Battery_bal.clear();
    }


}

void BMS::recordTempInfo(quint8 index, qint16 value)
{

    m_record_Temp_Index<<index;
    m_record_Temp_Time<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    m_record_Temp_v<<value;


    if(1000==m_record_Temp_Index.size()){
        QFile file(QCoreApplication::applicationDirPath()+"/tempfile.dat");
        qDebug()<<"有1000个数据了";
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QDataStream out(&file);
        for (int i=0;i<m_record_Temp_Index.size();i++) {
            out<<m_record_Temp_Index[i]<<m_record_Temp_Time[i]<<m_record_Temp_v[i];
        }
        m_record_Temp_Index.clear();
        m_record_Temp_Time.clear();
        m_record_Temp_v.clear();
    }
}

void BMS::recordOtherInfo(quint8 index, QVariant value)
{


    m_record_Other_Index<<index;
    m_record_Other_Time<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    m_storage_Other<<value;


    if(1000==m_record_Other_Index.size()){
        QFile file(QCoreApplication::applicationDirPath()+"/otherfile.dat");
        qDebug()<<"有1000个数据了";
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QDataStream out(&file);
        for (int i=0;i<m_record_Other_Index.size();i++) {
            out<<m_record_Other_Index[i]<<m_record_Other_Time[i]<<m_storage_Other[i];
        }
        m_record_Other_Index.clear();
        m_record_Other_Time.clear();
        m_storage_Other.clear();
    }
}

void BMS::export_record(QString fileUrl)
{

    for (int i=0;i<32;i++) {  //记录位置更新，重新为4行开始
        m_recordBatteryCnt[i]=4;
    }
    for (int i=0;i<8;i++) {  //记录位置更新，重新为4行开始
        m_recordTempCnt[i]=4;
    }
    for (int i=0;i<12;i++) {  //记录位置更新，重新为4行开始
        m_recordOtherCnt[i]=4;
    }
    QString filePath=fileUrl.mid(8);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)){
        emit exportRecordStatus("请关闭选择的导出文件或者检查文件是否存在！再执行导出操作");
        return;
    }

    file.close();

    QFile batteryFile(QCoreApplication::applicationDirPath()+"/batteryfile.dat");
    if(batteryFile.exists()){
        if (!batteryFile.open(QIODevice::ReadOnly)){
            emit exportRecordStatus("请关闭临时数据文件("+QCoreApplication::applicationDirPath()+"/batteryfile.dat)！再执行导出操作");
            return;
        }
    }

    QFile tempFile(QCoreApplication::applicationDirPath()+"/tempfile.dat");
    if(tempFile.exists()){
        if (!tempFile.open(QIODevice::ReadOnly)){
            emit exportRecordStatus("请关闭临时数据文件("+QCoreApplication::applicationDirPath()+"/tempfile.dat)！再执行导出操作");
            return;
        }
    }

    QFile otherFile(QCoreApplication::applicationDirPath()+"/otherfile.dat");
    if(otherFile.exists()){
        if (!otherFile.open(QIODevice::ReadOnly)){
            emit exportRecordStatus("请关闭临时数据文件("+QCoreApplication::applicationDirPath()+"/otherfile.dat)！再执行导出操作");
            return;
        }
    }

    QXlsx::Document xlsx;
    //    for(int i=0;i<32;i++){   //一页单个索引
    //        xlsx.addSheet("c"+QString::number(i+1));
    //    }
    for(int i=0;i<30;){    //一页多个索引，电池信息
        xlsx.addSheet("c"+QString::number(i+1)+"-"+QString::number(i+3));
        i=i+3;
    }
    xlsx.addSheet("c"+QString::number(31)+"-"+QString::number(32));


    xlsx.addSheet("T1-8");//一页多个索引，温度信息
    xlsx.addSheet("其他信息1");
    xlsx.addSheet("其他信息2");

    //    for(int i=0;i<32;i++){   //一页单个索引
    //        xlsx.selectSheet("c"+QString::number(i+1));
    //        xlsx.write("A1", "电池信息");
    //        xlsx.write("A2", "c"+QString::number(i+1));
    //        xlsx.write("A3", "时间");
    //        xlsx.write("B3", "数值/mv");
    //        xlsx.write("C3", "均衡");

    //    }

    //-----------------电池信息
    for(int i=0;i<30;){  //一页多个索引
        xlsx.selectSheet("c"+QString::number(i+1)+"-"+QString::number(i+3));
        xlsx.write("A1", "电池信息");
        xlsx.write("A2", "c"+QString::number(i+1));
        xlsx.write("A3", "时间");
        xlsx.write("B3", "数值/mv");
        xlsx.write("C3", "均衡");
        xlsx.write("E2", "c"+QString::number(i+2));
        xlsx.write("E3", "时间");
        xlsx.write("F3", "数值/mv");
        xlsx.write("G3", "均衡");
        xlsx.write("I2", "c"+QString::number(i+3));
        xlsx.write("I3", "时间");
        xlsx.write("J3", "数值/mv");
        xlsx.write("K3", "均衡");
        i=i+3;

    }
    xlsx.selectSheet("c"+QString::number(31)+"-"+QString::number(32));
    xlsx.write("A1", "电池信息");
    xlsx.write("A2", "c"+QString::number(31));
    xlsx.write("A3", "时间");
    xlsx.write("B3", "数值/mv");
    xlsx.write("C3", "均衡");
    xlsx.write("E2", "c"+QString::number(32));
    xlsx.write("E3", "时间");
    xlsx.write("F3", "数值/mv");
    xlsx.write("G3", "均衡");

    //-----------------温度信息
    xlsx.selectSheet("T1-8");
    xlsx.write("A1", "温度信息");
    QList<char> colTitle_temp{'A','D','G','J','M','P','S','V','Y'};
    for(int i=0;i<8;i++){  //一页多个索引
        xlsx.write(QString(colTitle_temp[i])+"2", "T"+QString::number(i+1));
        xlsx.write(QString(colTitle_temp[i])+"3", "时间");
        xlsx.write(QString(colTitle_temp[i]+1)+"3", "温度/℃");
    }

    //-----------------其他信息1
    xlsx.selectSheet("其他信息1");
    xlsx.write("A1", "其他信息");
    QList<QString> name_Other{"总电压","总电流","SOH估算值","最大需求充电电流","最高电压","最低电压","平均电压","电芯压差",
                              "循环次数"};
    QList<QString> unit_Other{"V","A","%","A","mV","mV","mV","mV",""};
    for(int i=0;i<9;i++){  //一页多个索引
        xlsx.write(QString(colTitle_temp[i])+"2", name_Other[i]);
        xlsx.write(QString(colTitle_temp[i])+"3", "时间");
        xlsx.write(QString(colTitle_temp[i]+1)+"3", "数值/"+unit_Other[i]);
    }

    //-----------------其他信息2
    xlsx.selectSheet("其他信息2");
    xlsx.write("A1", "其他信息");

    xlsx.write("A2", "满充容量");
    xlsx.write("A3", "时间");
    xlsx.write("B3", "数值/mAh");

    xlsx.write("D2", "剩余容量");
    xlsx.write("D3", "时间");
    xlsx.write("E3", "数值/mAh");

    xlsx.write("G2", "RSOC");
    xlsx.write("G3", "时间");
    xlsx.write("H3", "数值/%");

    //-------------------一页单个索引
    //    while (!file.atEnd()) {  //索引 时间 电压  均衡
    //        in>>index;
    //        qDebug()<<"index"<<index;
    //        xlsx.selectSheet("c"+QString::number(index+1));
    //        in>>time;
    //        qDebug()<<"time"<<time;
    //        xlsx.write("A"+QString::number(m_recordBatteryCnt[index]), time);
    //        in>>v;
    //        qDebug()<<"v"<<v;
    //        xlsx.write("B"+QString::number(m_recordBatteryCnt[index]), v);
    //        in>>bal;
    //        qDebug()<<"bal"<<bal;
    //        xlsx.write("C"+QString::number(m_recordBatteryCnt[index]), bal);
    //        m_recordBatteryCnt[index]++;

    //    }
    //    for (int i=0;i<m_record_Battery_Index.size();i++) { //读出容器中的剩余数据
    //        index=m_record_Battery_Index[i];
    //        xlsx.selectSheet("c"+QString::number(index+1));
    //        xlsx.write("A"+QString::number(m_recordBatteryCnt[index]), m_record_Battery_Time[i]);
    //        xlsx.write("B"+QString::number(m_recordBatteryCnt[index]), m_record_Battery_v[i]);
    //        xlsx.write("C"+QString::number(m_recordBatteryCnt[index]), m_record_Battery_bal[i]);
    //        m_recordBatteryCnt[index]++;
    //    }

    //-----------------一页多个索引-------------电池信息写入excel
    {
        quint8 index; //索引
        char colIndex;//列
        QString time;
        quint16 v;
        quint8 bal;

        if(batteryFile.exists()){ //如果文件存在的话
            QDataStream in(&batteryFile);
            while (!batteryFile.atEnd()) {  //索引 时间 电压  均衡
                in>>index;   //index 索引  如 第一页为 0 1 2   12
                quint8 index3=index%3;  //第一页余为 0 1 2     0
                colIndex='A';
                if(0==index3)
                    colIndex='A';
                else if (1==index3) {
                    colIndex='E';
                }
                else if (2==index3) {
                    colIndex='I';
                }
                if(index>=30)
                    xlsx.selectSheet("c"+QString::number(31)+"-"+QString::number(32));
                else
                    xlsx.selectSheet("c"+QString::number(index+1)+"-"+QString::number(index+3));
                in>>time;
                xlsx.write(colIndex+QString::number(m_recordBatteryCnt[index]), time);
                in>>v;
                xlsx.write((colIndex+1)+QString::number(m_recordBatteryCnt[index]), v);
                in>>bal;
                xlsx.write((colIndex+2)+QString::number(m_recordBatteryCnt[index]), bal);
                m_recordBatteryCnt[index]++;

            }  //文件读完

            batteryFile.close();
            batteryFile.remove();
        }
        for (int i=0;i<m_record_Battery_Index.size();i++) { //读出容器中的剩余数据
            index=m_record_Battery_Index[i];
            quint8 index3=index%3;
            colIndex='A';
            if(0==index3)
                colIndex='A';
            else if (1==index3) {
                colIndex='E';
            }
            else if (2==index3) {
                colIndex='I';
            }
            if(index>=30)
                xlsx.selectSheet("c"+QString::number(31)+"-"+QString::number(32));
            else
                xlsx.selectSheet("c"+QString::number(index+1)+"-"+QString::number(index+3));
            xlsx.write(colIndex+QString::number(m_recordBatteryCnt[index]), m_record_Battery_Time[i]);
            xlsx.write((colIndex+1)+QString::number(m_recordBatteryCnt[index]), m_record_Battery_v[i]);
            xlsx.write((colIndex+2)+QString::number(m_recordBatteryCnt[index]), m_record_Battery_bal[i]);
            m_recordBatteryCnt[index]++;
        }  //容器中数据读完

        m_record_Battery_Index.clear();
        m_record_Battery_Time.clear();
        m_record_Battery_v.clear();
        m_record_Battery_bal.clear();


    }

    //--------------------------------导出温度信息
    {
        quint8 index_temp; //索引
        QString time_temp;
        qint16 v_temp;
        xlsx.selectSheet("T1-8");

        if(tempFile.exists()){
            QDataStream in_temp(&tempFile);
            while (!tempFile.atEnd()) {  //索引 时间 温度
                in_temp>>index_temp;   //index 索引  如 第一页为 0 1 2   12
                in_temp>>time_temp;
                xlsx.write(colTitle_temp[index_temp]+QString::number(m_recordTempCnt[index_temp]), time_temp);
                in_temp>>v_temp;
                xlsx.write((colTitle_temp[index_temp]+1)+QString::number(m_recordTempCnt[index_temp]), v_temp);

                m_recordTempCnt[index_temp]++;

            }
            tempFile.close();
            tempFile.remove();
        }
        for (int i=0;i<m_record_Temp_Index.size();i++) { //读出容器中的剩余数据
            index_temp=m_record_Temp_Index[i];

            xlsx.write(colTitle_temp[index_temp]+QString::number(m_recordTempCnt[index_temp]), m_record_Temp_Time[i]);
            xlsx.write((colTitle_temp[index_temp]+1)+QString::number(m_recordTempCnt[index_temp]), m_record_Temp_v[i]);
            m_recordTempCnt[index_temp]++;
        }

        m_record_Temp_Index.clear();
        m_record_Temp_Time.clear();
        m_record_Temp_v.clear();


    }
    //--------------------------------其他信息
    {
        quint8 index_other; //索引
        QString time_other;
        QVariant v_other;

        if(otherFile.exists()){
            QDataStream in_other(&otherFile);
            while (!otherFile.atEnd()) {  //索引 时间 值
                in_other>>index_other;   //index 索引  如 第一页为 0 1 2   12
                in_other>>time_other;
                in_other>>v_other;

                if(index_other<9){  //记录在其他信息1中
                    xlsx.selectSheet("其他信息1");
                    xlsx.write(colTitle_temp[index_other]+QString::number(m_recordOtherCnt[index_other]), time_other);
                    if(0==index_other)
                        xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), v_other.toUInt()*0.01);

                    else if(1==index_other)
                        xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), v_other.toInt()*0.01);

                    else if(3==index_other)
                        xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), v_other.toUInt()*0.01);

                    else if(6==index_other)
                        xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), v_other.toDouble());

                    else {
                        xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), v_other.toUInt());
                    }
                }


                else{  //记录在其他信息2中 index_other 为9 10 11
                    xlsx.selectSheet("其他信息2");
                    xlsx.write(colTitle_temp[index_other-9]+QString::number(m_recordOtherCnt[index_other]), time_other);

                    if(9==index_other)
                        xlsx.write((colTitle_temp[index_other-9]+1)+QString::number(m_recordOtherCnt[index_other]), v_other.toUInt()*10);

                    else if(10==index_other)
                        xlsx.write((colTitle_temp[index_other-9]+1)+QString::number(m_recordOtherCnt[index_other]), v_other.toUInt()*10);
                    else
                        xlsx.write((colTitle_temp[index_other-9]+1)+QString::number(m_recordOtherCnt[index_other]), v_other.toUInt());
                }
                m_recordOtherCnt[index_other]++;
            }
            otherFile.close();
            otherFile.remove();
        }
        for (int i=0;i<m_record_Other_Index.size();i++) { //读出容器中的剩余数据
            index_other=m_record_Other_Index[i];

            if(index_other<9){  //记录在其他信息1中
                xlsx.selectSheet("其他信息1");
                xlsx.write(colTitle_temp[index_other]+QString::number(m_recordOtherCnt[index_other]), m_record_Other_Time[i]);
                if(0==index_other)
                    xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), m_storage_Other[i].toUInt()*0.01);

                else if(1==index_other)
                    xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), m_storage_Other[i].toInt()*0.01);

                else if(3==index_other)
                    xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), m_storage_Other[i].toUInt()*0.01);

                else if(6==index_other)
                    xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), m_storage_Other[i].toDouble());

                else {
                    xlsx.write((colTitle_temp[index_other]+1)+QString::number(m_recordOtherCnt[index_other]), m_storage_Other[i].toUInt());
                }
            }


            else{  //记录在其他信息2中 index_other 为9 10 11
                xlsx.selectSheet("其他信息2");
                xlsx.write(colTitle_temp[index_other-9]+QString::number(m_recordOtherCnt[index_other]), m_record_Other_Time[i]);

                if(9==index_other)
                    xlsx.write((colTitle_temp[index_other-9]+1)+QString::number(m_recordOtherCnt[index_other]), m_storage_Other[i].toUInt()*10);

                else if(10==index_other)
                    xlsx.write((colTitle_temp[index_other-9]+1)+QString::number(m_recordOtherCnt[index_other]), m_storage_Other[i].toUInt()*10);
                else
                    xlsx.write((colTitle_temp[index_other-9]+1)+QString::number(m_recordOtherCnt[index_other]), m_storage_Other[i].toUInt());

            }
            m_recordOtherCnt[index_other]++;

        }
        //------------------------------------------------------


        m_record_Other_Index.clear();
        m_record_Other_Time.clear();
        m_storage_Other.clear();


    }

    xlsx.saveAs(filePath);

    emit exportRecordStatus("导出记录文件成功！");
    //删除临时记录数据的文件

}



void BMS::getResponseMsg(QString s,bool isDialog) //得到通讯线程发出的信息
{
    qDebug()<<"getResponseMsg "<<s;
    if("通讯线程睡眠"==s){
        m_comThread.wakeUp();
        if(isDialog){
            qDebug()<<"isComSleep ";
            emit comSleep();
            //isComSleep=true;
            //m_comFlag=100;
            //getResponseData(QByteArray());
        }
    }
    else
        emit responseMsg(s,isDialog);

}

void BMS::getResponseTime(QString s)
{
    emit responseTime(s);
}

void BMS::getResponseData(QByteArray dataArray)
{
    qDebug()<<"getResponseData";
    qDebug()<<"comFlag"<<m_comFlag;
    //qDebug()<<dataArray;
    qDebug()<<dataArray.toHex();



    if(0==m_comFlag){  //只读一次的参数返回的内容
        m_batteryInfoModel->clear();
        m_tempInfoModel->clear();
        m_otherInfoModel->clear();

        QList<QString> nameList{"总电压","总电流","SOH估算值","最大需求充电电流","最高电压","最低电压","平均电压","电芯压差","循环次数","满充容量","剩余容量","RSOC"};
        QList<QString> unitList{"V","A","%","A","mV","mV","mV","mV","--","mAh","mAh","%"};
        for (int i=0;i<nameList.size();i++) {
            m_otherInfoModel->addOtherInfo(nameList[i],unitList[i]);
        }

        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        //读回来的内容是读No2~No18,接下来循环读电池信息

        m_batteryCnt=((quint16)dataArray.at(37)<<8)+(quint8)dataArray.at(36);  //36=7+(16-2)*2+1 7:帧头 16:No16 +1:pre
        for(int i=0;i<m_batteryCnt;i++)
            m_batteryInfoModel->addBatteryInfo("C"+QString::number(i+1),0,false);


        m_tempCnt=((quint16)dataArray.at(41)<<8)+(quint8)dataArray.at(40);  //40=7+(18-2)*2+1
        for(int i=0;i<m_tempCnt;i++)
            m_tempInfoModel->addTempInfo("T"+QString::number(i+1),0);

        qDebug()<<"m_batteryCnt"<<"m_tempCnt"<<m_batteryCnt<<m_tempCnt;


        //计算 No10 24=7+(10-2)*2+1
        quint16 p_dateYear=(((quint16)dataArray.at(25)&0xfe)>>1)+2000;
        quint16 p_dateMonth=(((quint16)dataArray.at(25)&0x01)<<3)+(((quint8)dataArray.at(24)&0xE0)>>5);
        quint8 p_dateDay=(quint8)dataArray.at(24)&0x1F;
        qDebug()<<"日期"<<p_dateYear<<p_dateMonth<<p_dateDay;
        QString p_date=QString::number(p_dateYear)+"-"+QString::number(p_dateMonth)+"-"+QString::number(p_dateDay);

        //计算 No12 28=7+(12-2)*2+1
        QString afe_s;
        quint16 afe_n=(quint16)dataArray.at(28)&0x0f;
        if(4==afe_n){
            afe_s="TI-76930";
        }
        else if(8==afe_n){
            afe_s="中颖-367309";
        }
        emit getAFE_S(afe_s);
        //计算 No13 30=7+(13-2)*2+1
        QString h_ver;
        quint16 h_ver_h=(quint16)dataArray.at(31)&0x0f;
        quint16 h_ver_m=((quint16)dataArray.at(30)&0xf0)>>4;
        quint16 h_ver_l=(quint16)dataArray.at(30)&0x0f;
        h_ver="V"+QString::number(h_ver_h)+"."+QString::number(h_ver_m)+"."+QString::number(h_ver_l);
        qDebug()<<"h_ver"<<h_ver;


        //计算 No14 32=7+(14-2)*2+1
        QString s_ver;
        quint16 s_ver_h=(quint16)dataArray.at(33)&0x0f;
        quint16 s_ver_m=((quint16)dataArray.at(32)&0xf0)>>4;
        quint16 s_ver_l=(quint16)dataArray.at(32)&0x0f;
        s_ver="V"+QString::number(s_ver_h)+"."+QString::number(s_ver_m)+"."+QString::number(s_ver_l);
        qDebug()<<"s_ver"<<s_ver;

        //计算 No15 34=7+(15-2)*2+1  高字节：电芯厂家 低字节：Pack厂家
        QString bat_type;
        QString pack_type;
        quint16 bat_type_h=(quint8)dataArray.at(35);
        quint16 bat_type_l=(quint8)dataArray.at(34);
        if(1==bat_type_h){
            bat_type="ATL";
        }
        else if(2==bat_type_h){
            bat_type="星恒";
        }
        else if(3==bat_type_h){
            bat_type="亿纬";
        }
        else {
            bat_type="ERROR";
        }

        if(1==bat_type_l)
            pack_type="飞毛腿";
        else if(2==bat_type_l)
            pack_type="星恒";
        else if(3==bat_type_l)
            pack_type="ATL";
        else
            pack_type="ERROR";



        m_commandList.clear();
        m_commandList<<QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("01620f"));

        if(m_batteryCnt<23){
            m_commandList<<QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("0195")).append(QByteArray::fromHex(QByteArray::number((49+2*m_batteryCnt),16)));
            m_commandList<<QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("01c90f"));
        }
        else{
            m_commandList<<QByteArray::fromHex("5aa501").append(m_currentSender).append(m_currentReceiver).append(QByteArray::fromHex("019577"));
        }

        m_comFlag++;
        m_comThread.sendChanged(m_commandList,true,m_comFlag,m_batteryCnt);
        emit no2_15(dataArray.mid(8,16),p_date,afe_s,h_ver,s_ver,bat_type,pack_type);
    }
    else if(1==m_comFlag){  //每秒读参数返回的内容


        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        if(dataArray.at(0)==0){
            //返回的是No89~No91的内容
            QList<quint16> tempList;
            for(int i=0;i<6;){
                tempList<<((quint16)dataArray.at(i+9)<<8)+(quint8)dataArray.at(i+8);
                i=i+2;
            }
            // index 6 7 8
            if(m_otherInfoModel->getScan(8))
                m_otherInfoModel->setData(m_otherInfoModel->getIndex(8,1),tempList[0],OtherInfoModel::vRole);
            if(m_otherInfoModel->getScan(9))
                m_otherInfoModel->setData(m_otherInfoModel->getIndex(9,1),tempList[1]*10,OtherInfoModel::vRole);
            if(m_otherInfoModel->getScan(10))
                m_otherInfoModel->setData(m_otherInfoModel->getIndex(10,1),tempList[2]*10,OtherInfoModel::vRole);

            m_record_Other[8]=QVariant(tempList[0]);
            m_record_Other[9]=QVariant(tempList[1]);
            m_record_Other[10]=QVariant(tempList[2]);

        }
        else if(dataArray.at(0)==1){
            if(m_batteryCnt<23){ //当电池串数小于23时
                //返回的是No140~电池串结束的内容
                //140 141 143 146 147 150 160
                cookNo140_1x(dataArray);

            }
            else{//当电池串数大于等于23时
                //返回的是No140~No194的内容
                qDebug()<<"大于等于23";
                //140 141 143 146 147 150 160 192~194
                cookNo140_1x(dataArray);
                //No192  计算 No192 112=7+(192-140)*2+1
                cookNo192_194(dataArray,112);
            }
        }
        else if(dataArray.at(0)==2){
            //返回的是No192~No194的内容
            //qDebug()<<"No192~No194的内容"<<dataArray;
            //No192  计算 No192 8=7+(192-192)*2+1
            cookNo192_194(dataArray,8);
        }

    }

    else if(m_comFlag>=2 && m_comFlag<=9){
        //回复的数据是写参数No19-No31
        if(dataArray.right(dataArray.size()-1).left(7)==m_paramPre[m_comFlag-2]){
            //写No19-No31正确
            emit isWriteParamOk(true);
            //写参数No33-No34
            if(m_comFlag<9)
                paramConfig(m_comFlag+1);
            else if(9==m_comFlag)
                paramConfig(30);
        }
        else {
            emit isWriteParamOk(false);
        }
    }
    else if(30==m_comFlag){ //写参数no116-118的返回值
        if(dataArray.right(dataArray.size()-1).left(7)==m_paramPre[m_paramPre.size()-1]){
            emit isWriteParamOk(true);
        }
        else {
            emit isWriteParamOk(false);
        }
    }
    //第一次读回来的数据，即comFlag=0时，数据为电池串的个数
    //quint8 cnt=dataArray.at(6)*256+dataArray.at(7);
    //for(int i=0;i<cnt;i++)
    //    batteryInfoModel->addBatteryInfo("c"+QString::number(i));
    else if(10==m_comFlag){
        //读取参数
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        qDebug()<<"-9之后"<<dataArray.toHex();
        QList<quint16> temp;
        qDebug()<<(quint16)dataArray.at(8);
        qDebug()<<(quint16)dataArray.at(9);
        for(int i=8;i<(dataArray.size()-2);){
            temp<<((quint16)dataArray.at(i+1)<<8)+(quint8)dataArray.at(i);
            //qDebug()<<temp;
            i=i+2;
        }
        paramInCap(temp[0]*10,0);
        paramInCap(temp[71]*10,1);
        paramInCap(temp[1]*0.01,2);
        for(int i=0;i<10;i++){
            paramInCap(temp[i+2],6+i);
        }
        paramInCap(temp[14],3);
        paramInCap(temp[15],4);
        paramInCap(temp[17],5);

        paramInCharge(temp[97]*0.01,0);
        paramInCharge(temp[98],1);
        paramInCharge(temp[99],2);

        paramInBasic1(temp[25],0);
        paramInBasic1(temp[28],1);
        paramInBasic1(temp[19]*0.1,2);
        paramInBasic1(temp[22]*0.1,3);
        paramInBasic1((qint16)temp[48],4);
        paramInBasic1((qint16)temp[52],5);
        paramInBasic1((qint16)temp[46],6);
        paramInBasic1((qint16)temp[50],7);
        paramInBasic1(temp[35]*0.1,8);
        paramInBasic1(temp[37],9);

        paramInBasic2(temp[26],0);
        paramInBasic2(temp[29],1);
        paramInBasic2(temp[20],2);
        paramInBasic2(temp[23],3);
        paramInBasic2(temp[36]&0x3f,8);
        paramInBasic2(temp[38],9);

        paramInBasic3(temp[27],0);
        paramInBasic3(temp[30],1);
        qDebug()<<"no40"<<temp[21]<<temp[21]*0.1;
        paramInBasic3(temp[21]*0.1,2);
        paramInBasic3(temp[24]*0.1,3);
        paramInBasic3((qint16)temp[49],4);
        paramInBasic3((qint16)temp[53],5);
        paramInBasic3((qint16)temp[47],6);
        paramInBasic3((qint16)temp[51],7);

        paramInAdvanced(temp[40],0);
        paramInAdvanced(temp[43],1);
        paramInAdvanced((temp[36]&0xc0)>>6,2);
        paramInAdvanced((temp[39]&0xc0)>>6,3);
        paramInAdvanced((temp[42]&0xc0)>>6,4);
        paramInAdvanced((temp[45]&0xc0)>>6,5);
        paramInResistor(temp[67]*0.1,6);

        paramInAdvanced2(temp[41],0);
        paramInAdvanced2(temp[44],1);

        paramInOther(temp[61],0);
        paramInOther(temp[62],1);
        paramInOther(temp[63],2);
        paramInOther(temp[31],3);
        paramInOther(temp[32],4);
        paramInOther(temp[33],5);
        paramInOther((qint16)temp[79]*(-1),6);

        paramInOther2(temp[34],0);
        paramInOther2(temp[75],1);
        paramInOther2(temp[76],2);
        paramInOther2(temp[77],3);
        paramInOther2(temp[78],4);
        paramInOther2(temp[80],6);

    }

    else if(m_comFlag>=11 && m_comFlag<=18 ){ //返回校准温度的值
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        qint16 temp=((qint16)dataArray.at(12)<<8)+(quint8)dataArray.at(11);

        qDebug()<<"校准返回值 temp"<<temp;
        emit getCorrectTemp(temp,m_comFlag-10);
        int j=m_comFlag-10;
        for (;j<8;j++) {
            if(m_correctTempIndex[j]){
                m_correctTempIndex[j]=false;
                correctTemp(j+1,m_correctTempValue[j]);
                return;
            }

        }
        correct_readTemp();

    }
    else if(19==m_comFlag || 20==m_comFlag){
        //返回校准电流的值
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        qint16 a=((qint16)dataArray.at(12)<<8)+(quint8)dataArray.at(11);
        qDebug()<<"校准电流"<<a;
        emit getCorrectA(a,m_comFlag-19);

    }

    //
    else if (22==m_comFlag) {//得到no101_115的值
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        //void no101_109(quint16 v);
        //void no110_113(qint16 v);
        //void no114_115(quint16 v);
        quint8 index=0;
        for(int i=8;i<(dataArray.size()-2);){  //No101 8=7+(101-101)*2+1
            if(i<26){
                emit no101_109(((quint16)dataArray.at(i+1)<<8)+(quint8)dataArray.at(i),index);
                index++;
            }
            else if(i<34){
                emit no110_113(((qint16)dataArray.at(i+1)<<8)+(quint8)dataArray.at(i),index);
                index++;
            }
            else {
                emit no114_115(((quint16)dataArray.at(i+1)<<8)+(quint8)dataArray.at(i),index);
                index++;
            }
            i=i+2;

        }
    }
    else if (23==m_comFlag) { //返回的是rtcTime
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        qDebug()<<"rtctime"<<dataArray.toHex();
        quint8 sec=dataArray[11]&0x3f;
        quint8 minute=(((quint16)dataArray[12]&0x0f)<<2)+(((quint16)dataArray[11]&0xc0)>>6);
        quint8 hour=(((quint16)dataArray[13]&0x1)<<4)+(((quint16)dataArray[12]&0xf0)>>4);
        quint8 day=((quint8)dataArray[13]&0x3e)>>1;
        quint8 mon=(((quint16)dataArray[14]&0x3)<<2)+(((quint16)dataArray[13]&0xc0)>>6);
        quint16 year=(((quint16)dataArray[14]&0xfc)>>2)+2000;
        QString s=QString::number(year)+"-"+QString::number(mon).prepend("0").right(2)+"-"+QString::number(day).prepend("0").right(2)
                +"-"+QString::number(hour).prepend("0").right(2)+":"+QString::number(minute).prepend("0").right(2)+":"+QString::number(sec).prepend("0").right(2);

        qDebug()<<"时间"<<s;
        emit rtcTime(s);
    }
    else if (24==m_comFlag) {//返回的是日志数量
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        quint16 logCnt=((quint16)dataArray.at(12)<<8)+(quint8)dataArray.at(11);
        qDebug()<<"logCnt"<<logCnt;
        m_logCnt=logCnt;
        emit showLogCnt(logCnt);
        getLog();

    }
    else if (25==m_comFlag) {//得到日志
        m_readLogCnt++;
        qDebug()<<"得到日志1";
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        qDebug()<<dataArray.toHex();
        QList<QString> eventTypeList={"开始充电","结束充电","欠压","过压","温度故障","电流故障","硬件故障","安全故障"};
        QString eventType=eventTypeList[(quint8)dataArray.at(11)];
        quint8 sec=dataArray[12]&0x3f;
        quint8 minute=(((quint16)dataArray[13]&0x0f)<<2)+(((quint16)dataArray[12]&0xc0)>>6);
        quint8 hour=(((quint16)dataArray[14]&0x1)<<4)+(((quint16)dataArray[13]&0xf0)>>4);
        quint8 day=((quint8)dataArray[14]&0x3e)>>1;
        quint8 mon=(((quint16)dataArray[15]&0x3)<<2)+(((quint16)dataArray[14]&0xc0)>>6);
        quint16 year=(((quint16)dataArray[15]&0xfc)>>2)+2000;

        QString date=QString::number(year)+"-"+QString::number(mon).prepend("0").right(2)+"-"+QString::number(day).prepend("0").right(2)
                +"-"+QString::number(hour).prepend("0").right(2)+":"+QString::number(minute).prepend("0").right(2)+":"+QString::number(sec).prepend("0").right(2);

        QString date2="y"+QString::number(year)+"m"+QString::number(mon)+"d"+QString::number(day)
                +"h"+QString::number(hour)+":"+QString::number(minute)+":"+QString::number(sec);


        //QString 转为QDateTime
        // QDateTime dateTime2 = QDateTime::fromString("M1d1y9800:01:02",
        //        "'M'M'd'd'y'yyhh:mm:ss");
        // dateTime is 1 January 1998 00:01:02

        QDateTime dateTime = QDateTime::fromString(date2,"'y'yyyy'm'M'd'd'h'h:m:s");
        qDebug()<<dateTime;



        double totalV=(((quint16)dataArray.at(17)<<8)+(quint8)dataArray.at(16))*0.01;
        double totalI=(((quint16)dataArray.at(19)<<8)+(quint8)dataArray.at(18))*0.01;
        qint8 temp=(qint8)dataArray.at(20);
        quint32 cap=(((quint16)dataArray.at(22)<<8)+(quint8)dataArray.at(21))*10;
        quint32 leftCap=(((quint16)dataArray.at(24)<<8)+(quint8)dataArray.at(23))*10;
        QByteArray bmsStatus=dataArray.mid(25,4).toHex();
        if(m_readLogCnt<m_logCnt){
            rxLogInfoList<<LogInfo(eventType,dateTime,totalV,totalI,temp,cap,leftCap,bmsStatus);
            if(m_readLogCnt%10==0){
                m_logInfoModel->addAllLogInfo(rxLogInfoList);
                rxLogInfoList.clear();
                emit readLogOnePage();
            }
        }

        else if(m_readLogCnt==m_logCnt){
            rxLogInfoList<<LogInfo(eventType,dateTime,totalV,totalI,temp,cap,leftCap,bmsStatus);
            //日志已经接收完毕
            m_readLogCnt=0;
            m_logInfoModel->addAllLogInfo(rxLogInfoList);
            rxLogInfoList.clear();
            qDebug()<<"日志读取完毕"<<m_logCnt<<std::ceil((double)m_logCnt/10);
            //发出信号
            emit readLogOnePage();
        }
    }
    else if(26==m_comFlag){ //删除ROM日志的回复
        if(10==(quint8)dataArray[11]){
            //删除成功
            emit deleteROMLog(true);
        }
        else if (11==(quint8)dataArray[11]) {
            //删除失败
            emit deleteROMLog(false);
        }

    }
    else if (28==m_comFlag) {//读Afe的回复
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        quint8 temp1=0;
        qint8 temp2=0;
        QString temp3="";
        for(int i=8;i<36;i++){
            if(i<10){  //显示为hex
                qDebug()<<"QString(dataArray.mid(i,1).toHex())"<<QString(dataArray.mid(i,1).toHex());
                qDebug()<<dataArray.mid(i,1).toHex();
                temp3=QString(dataArray.mid(i,1).toHex())+"H";
                qDebug()<<temp3;
                valueInAfeHex(i-8,temp3);
            }
            else if(i<25){
                temp1=(quint8)dataArray[i];
                valueInAfe(i-8,temp1);
            }
            else if (i<33) {
                temp2=(qint8)dataArray[i];
                valueInAfe(i-8,temp2);
                qDebug()<<"temp2.toInt()"<<temp2;
            }
            else if(34==i){
                temp3=QString(dataArray.mid(i,1).toHex())+"H";
                qDebug()<<temp3;
                valueInAfeHex(i-8,temp3);
            }
            else{
                temp1=(quint8)dataArray[i];
                valueInAfe(i-8,temp1);
            }

        }
    }

    else if (29==m_comFlag) { //校准页读实时温度值和实时电流的回复 最前为0 为返回的实时温度 为1 为返回的电流值
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        if(0==(quint8)dataArray[0]){ //返回的是实时温度值
            QList<qint16> tempList;
            for(int i=8;i<(dataArray.size()-2);){  //
                tempList<<((qint16)dataArray.at(i+1)<<8)+(quint8)dataArray.at(i);
                i=i+2;
            }
            qDebug()<<"tempList.size()"<<tempList.size();
            for (int i=0;i<tempList.size();i++) {
                emit getCorrectRealTemp(i,tempList[i]);
            }

        }
        else if (1==(quint8)dataArray[0]) {
            qint16 v=((qint16)dataArray.at(9)<<8)+(quint8)dataArray.at(8);
            emit no141(v*0.01);
        }

    }

    else if(31==m_comFlag){ //电池信息页返回校准控制命令的值
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        quint16 v=((quint16)dataArray.at(12)<<8)+(quint8)dataArray.at(11);
        emit getCorrectControl(v);
    }
    else if(32==m_comFlag){ //电池信息页返回mos命令
        for(int i=1;i<dataArray.size();i++){
            dataArray[i]=dataArray[i]-9;
        }
        qDebug()<<"dataArray.at(11)"<<(quint8)dataArray.at(11);
        quint8 v=(quint8)dataArray.at(11);
        qDebug()<<"dataArray.at(11)"<<v;
        emit getCorrectMos(v);
    }

    emit comSuccessfully();

//    if(isComSleep){
//        isComSleep=false;
//         emit comSleep();
//    }

}
void BMS::cookNo140_1x(QByteArray& dataArray)
{
    QList<quint16> tempList;
    QList<qint16> tempList2;
    for(int i=0,j=140;j<(160+m_batteryCnt);j++){  //No140,No141 i=7+(140-140)*2+1
        if(j==140 || j==142 || j==143 || j==144 || j==146 || j==147 ||j==158||j==159 ||(j>=160 && j<(160+m_batteryCnt))){
            tempList<<((quint16)dataArray.at(i+9)<<8)+(quint8)dataArray.at(i+8);
        }
        if(j==141  || (j>=150 && j<(150+m_tempCnt))){
            tempList2<<((qint16)dataArray.at(i+9)<<8)+(quint8)dataArray.at(i+8);
        }
        i=i+2;
    }
    qDebug()<<tempList.size();

    emit no145_147(tempList[2],tempList[4],tempList[5]);

    //140 141 143
    if(m_otherInfoModel->getScan(0))
        m_otherInfoModel->setData( m_otherInfoModel->getIndex(0,1),tempList[0]*0.01,OtherInfoModel::vRole);
    if(m_otherInfoModel->getScan(1))
        m_otherInfoModel->setData( m_otherInfoModel->getIndex(1,1),tempList2[0]*0.01,OtherInfoModel::vRole);

    if(m_otherInfoModel->getScan(2))
        m_otherInfoModel->setData( m_otherInfoModel->getIndex(2,1),tempList[1],OtherInfoModel::vRole);
    if(m_otherInfoModel->getScan(3))
        m_otherInfoModel->setData( m_otherInfoModel->getIndex(3,1),tempList[3]*0.01,OtherInfoModel::vRole);

    if(m_otherInfoModel->getScan(11))
        m_otherInfoModel->setData( m_otherInfoModel->getIndex(11,1),tempList[2],OtherInfoModel::vRole);

    m_record_Other[0]=QVariant(tempList[0]);
    m_record_Other[1]=QVariant(tempList2[0]);
    m_record_Other[2]=QVariant(tempList[1]);
    m_record_Other[3]=QVariant(tempList[3]);
    m_record_Other[11]=QVariant(tempList[2]);
    //j==158 j==159  为均衡状态   j==158 tempList[4]高16串    j==159 tempList[5]低16串

    quint32 sumV=0;
    QList<bool> balList;
    QList<quint16> vList;

    //----------bal
    qDebug()<<"bal"<<tempList[6]<<tempList[7];
    for(int i=0;i<16;i++){
        balList<<((tempList[6]&(1<<i))!=0);
    }
    for(int i=0;i<16;i++){
        balList<<((tempList[7]&(1<<i))!=0);
    }
    qDebug()<<balList.size();
    for (int i=0;i<balList.size();i++) {
        qDebug()<<balList[i];
    }
    for(int i=8;i<tempList.size();i++){  //电池电压值
        if(m_batteryInfoModel->getScan(i-8)){  //要扫描
            m_batteryInfoModel->setData( m_batteryInfoModel->getIndex(i-8,1),tempList[i],BatteryInfoModel::vRole);
            m_batteryInfoModel->setData( m_batteryInfoModel->getIndex(i-8,2),balList[i-8],BatteryInfoModel::balRole);
            vList<<tempList[i];
            sumV+=tempList[i];
        }
    }
    for(int i=1;i<tempList2.size();i++){ //电池温度值
        if(m_tempInfoModel->getScan(i-1)){ //要扫描
            m_tempInfoModel->setData( m_tempInfoModel->getIndex(i-1,1),tempList2[i],TempInfoModel::tempRole);
        }
    }

    //-----------------------------------------
    quint16 maxV=*(std::max_element(vList.begin(),vList.end()));
    quint16 minV=*(std::min_element(vList.begin(),vList.end()));
    double averageV=(double)sumV/vList.size();
    quint16 diffV=maxV-minV;
    if(m_otherInfoModel->getScan(4))
        m_otherInfoModel->setData( m_otherInfoModel->getIndex(4,1),maxV,OtherInfoModel::vRole);
    if(m_otherInfoModel->getScan(5))
        m_otherInfoModel->setData( m_otherInfoModel->getIndex(5,1),minV,OtherInfoModel::vRole);
    if(m_otherInfoModel->getScan(6))
        m_otherInfoModel->setData( m_otherInfoModel->getIndex(6,1),averageV,OtherInfoModel::vRole);
    if(m_otherInfoModel->getScan(7))
        m_otherInfoModel->setData( m_otherInfoModel->getIndex(7,1),diffV,OtherInfoModel::vRole);


    m_record_Other[4]=QVariant(maxV);
    m_record_Other[5]=QVariant(minV);
    m_record_Other[6]=QVariant(averageV);
    m_record_Other[7]=QVariant(diffV);

    //--------------------记录------------------------
    m_rx_cnt++;
    if(m_rx_cnt==m_record_Rx_Scale){
        //到记录数据的次数
        m_rx_cnt=0;
        //-----------如果需要记录数据，则记录----------
        //-----------记录电池信息----------
        for (int i=8;i<tempList.size();i++) {
            if(m_batteryInfoModel->getRecord(i-8)){
                //需要记录
                recordBatteryInfo(i-8,tempList[i],balList[i-8]);
            }
        }
        //----------记录温度信息----tempList2从1开始-------
        for (int i=1;i<tempList2.size();i++) {
            if(m_tempInfoModel->getRecord(i-1)){
                //需要记录
                recordTempInfo(i-1,tempList2[i]);
            }
        }
        //----------记录其他信息----m_record_Other-------
        for (int i=0;i<12;i++) {
            if(m_otherInfoModel->getRecord(i)){
                //需要记录
                recordOtherInfo(i,m_record_Other[i]);
            }
        }
    }

}

void BMS::cookNo192_194(QByteArray& dataArray,quint8 index)
{
    quint8 mos_d=(quint8)dataArray.at(index)&0x01;
    quint8 mos_c=(quint8)dataArray.at(index)&0x02;
    quint8 mos_p=(quint8)dataArray.at(index)&0x04;

    qDebug()<<"mos"<<mos_d<<mos_c<<mos_p;
    //No193  计算 No193 10=7+(193-192)*2+1
    quint16 piso_r=((quint16)dataArray.at(index+3)<<8)+(quint8)dataArray.at(index+2);
    //No194  计算 No194 12=7+(194-192)*2+1
    quint16 niso_r=((quint16)dataArray.at(index+5)<<8)+(quint8)dataArray.at(index+4);

    emit no192_194(mos_d!=0,mos_c!=0,mos_p!=0,piso_r,niso_r);
}

void BMS::setComFailBool(bool continueCom)
{
    m_continueCom=continueCom;
    m_comThread.setComFailAction(m_comFailCnt,m_continueCom);
}

void BMS::setComFailCnt(quint32 comFailCnt)
{
    m_comFailCnt=comFailCnt;
    m_comThread.setComFailAction(m_comFailCnt,m_continueCom);
}
