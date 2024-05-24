#ifndef BMS_H
#define BMS_H

#include <QObject>
#include "DocObjectService.h"
#include "batteryinfo.h"
#include "tempinfo.h"
#include "otherinfomodel.h"
#include "loginfomodel.h"
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include "comthread.h"
using namespace QXlsx;

class BMS : public QObject
{
    Q_OBJECT
public:
    explicit BMS(QObject *parent = 0);
    Q_INVOKABLE void transaction(const QString &portName,const uint &baudRate,const int& dataBits,
                                 const int &stopBits,const QString &parity,
                                 const quint8 & index);//index为打开串口时所在的界面，是电池信息界面还是参数、校准等别的界面
    Q_INVOKABLE void closeSerialPort();
    Q_INVOKABLE void setBatteryInfoModel(QObject* obj);
    Q_INVOKABLE void setTempInfoModel(QObject* obj);
    Q_INVOKABLE void setLogInfoModel(QObject* obj);
    Q_INVOKABLE void setOtherInfoModel(QObject* obj);

    Q_INVOKABLE void stopReadInfo();

    Q_INVOKABLE void readBatteryInfo(bool includeHead);//恢复每秒读电池参数

    Q_INVOKABLE void paramConfig(quint8 comFlag);
    Q_INVOKABLE void readParamConfig();  //通讯读取参数

    Q_INVOKABLE void saveParam(const QVariant& param,const quint8& indexRow,const quint8& indexCol,const QString& viewName);
    Q_INVOKABLE void exportParam(QString filePath,const QList<QString>& nameInCapModel,const QList<QString>& nameInBasicModel, const QList<QString>&name3InBasicModel
                                 ,const QList<QString>&nameInAdvanced,const QList<QString>& nameInOther,const QList<QString>& name2InOther);
    Q_INVOKABLE void importParam(QString filePath);

    //---------------------校准页
    Q_INVOKABLE void correct_readTemp(); //读实时温度信息和实时总电流信息

    Q_INVOKABLE void set_correctTemp(quint8 index,bool value,qint16 temp); //value:是否需要校准  temp:校准值
Q_INVOKABLE void start_correctTemp(); //开始温度校准

    Q_INVOKABLE QString correctDate();
    Q_INVOKABLE void correctTemp(quint8 num,qint16 value);//num 校准的哪一个温度  value 校准的温度值
    Q_INVOKABLE void correctA(quint8 num,qint16 value);//num 校准的哪一个电流  value 校准的电流值

    Q_INVOKABLE void correctControl(QString value);//电池信息页 校准控制命令
    Q_INVOKABLE void correctMos(int e_v);


    Q_INVOKABLE void getNo101_115();  //日志页需获取No101-115的参数
    Q_INVOKABLE void getRtcTime();  //日志页需获取rtc时间
    Q_INVOKABLE void getLogCnt();  //读取日志数量

    Q_INVOKABLE void getLog();  //读取日志,cnt为日志数量
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void deleteRomLog();  //清空rom中的日志

    Q_INVOKABLE void changeDelay(quint16 waitRx_MS,quint16 waitRx_byte_MS,quint16 rxFailWait_MS,quint16 rxWait_MS);

    //-------------------afe页
    Q_INVOKABLE void saveAfe(int row,int col,qint8 value);
    Q_INVOKABLE void saveAfeHex(int row,int col,QString value);
    Q_INVOKABLE void exportAfe(QString filePath,const QList<QString>& nameInAfeModel,const QList<QString>& name2InAfeModel, const QList<QString>&name3InAfeModel);
    Q_INVOKABLE void importAfe(QString filePath);
    Q_INVOKABLE void writeAfe();
    Q_INVOKABLE void readAfe();


    //--------------------记录

    //-------------------记录间隔
    Q_INVOKABLE void record_Rx_Scale(quint32 scanTime,quint32 recordTime);

    //-------------------记录电压信息
    void recordBatteryInfo(quint8 index,quint16 value,quint8 bal);

    //-------------------记录温度信息
    void recordTempInfo(quint8 index,qint16 value);

    //-------------------记录其他信息
    void recordOtherInfo(quint8 index,QVariant value);


    Q_INVOKABLE void export_record(QString fileUrl); //导出记录信息




    void cookNo140_1x(QByteArray& dataArray);
    void cookNo192_194(QByteArray& dataArray,quint8 index);


    //--------------------通讯失败处理
    Q_INVOKABLE void setComFailCnt(quint32 comFailCnt);
    Q_INVOKABLE void setComFailBool(bool continueCom);




signals:
    void responseMsg(const QString s,bool isDialog);//isDialog:true 对话框提示  false：comStatus提示

    void no2_15(QString v1,QString v2,QString v3,QString v4,QString v5,QString v6,QString v7);
    void no145_147(quint16 v1,quint16 v2,quint16 v3);
    void no192_194(bool v1,bool v2,bool v3,int v4,int v5);

    void loadExcelFail();

    //import param
    void paramInCap(QVariant param,quint8 index);  //从excel读取的容量参数的值
    void paramInBasic1(QVariant param,quint8 index);  //从excel读取的容量参数的值
    void paramInBasic2(QVariant param,quint8 index);
    void paramInBasic3(QVariant param,quint8 index);
    void paramInAdvanced(QVariant param,quint8 index);
    void paramInAdvanced2(QVariant param,quint8 index);
    void paramInOther(QVariant param,quint8 index);
    void paramInOther2(QVariant param,quint8 index);
    void paramInResistor(QVariant param,quint8 index);
    void paramInCharge(QVariant param,quint8 index);

    void isWriteParamOk(bool status);
    void getAFE_S(QString s);

    void getCorrectTemp(qint16 value,quint8 num);//value值 num代表哪一个温度值
    void no141(double value);
    void getCorrectA(qint16 value,quint8 num);//value值 num代表哪一个电流值
    void rtcTime(QString s);
    void getCorrectControl(quint16 value);
    void getCorrectMos(quint16 value);
    void responseTime(QString s);

    void no101_109(quint16 v,quint8 index); //v值 index行索引
    void no110_113(qint16 v,quint8 index);
    void no114_115(quint16 v,quint8 index);



    void readLogOnePage();//pageCnt为显示日志的页数，每页是十条
    void showLogCnt(quint32 cnt);  //显示日志数量

    void deleteROMLog(bool b);

    //---------afe信号
    void valueInAfe(int index,QVariant value);
    void valueInAfeHex(int index,QString value);

    //---------校准页，实时温度
    void getCorrectRealTemp(quint8 index,qint16 value);

    void comSleep();

    //-------------导出记录
    void exportRecordStatus(QString s);

    void comSuccessfully();

public slots:

private slots:

    void getResponseData(QByteArray dataArray);
    void getResponseMsg(QString s,bool isDialog);
    void getResponseTime(QString s);

private:
    ComThread m_comThread;
    quint8 m_comFlag;//通讯标志,看现在通讯的是什么内容，以决定如何处理获得的数据
    BatteryInfoModel* m_batteryInfoModel;
    TempInfoModel* m_tempInfoModel;
    LogInfoModel* m_logInfoModel;
    OtherInfoModel* m_otherInfoModel;

    QList<QByteArray> m_commandList;

    quint8 m_batteryCnt;//电池串数
    quint8 m_tempCnt;//电芯温度数量
    QByteArray m_currentSender;//
    QByteArray m_currentReceiver;

    QList<quint16> m_param19_30;  //写入参数成功后清空  写入的数据应当是传输的数据
    QList<quint16> m_param33_34;  //写入参数成功后清空
    QList<quint16> m_param36;  //写入参数成功后清空
    QList<quint16> m_param38_72;  //写入参数成功后清空
    QList<quint16> m_param80_82;  //写入参数成功后清空
    QList<quint16> m_param86;  //写入参数成功后清空
    QList<quint16> m_param90;  //写入参数成功后清空
    QList<quint16> m_param94_99;  //写入参数成功后清空
    QList<quint16> m_param116_118;

    quint8 m_paramGIndex;//写入的是以上哪一组参数
    QList<QByteArray> m_paramPre; //写参数数据帧前缀

    quint16 m_logCnt;//日志的数量
    quint16 m_readLogCnt;
    quint8 m_currentIndex;

    QList<LogInfo> rxLogInfoList;

    QList<quint8> m_afeValue;


    //---------------记录
    QList<quint8> m_record_Battery_Index; //记录电池信息的index
    QList<QString> m_record_Battery_Time; //记录电池信息的time
    QList<quint16> m_record_Battery_v;//记录电池信息的电压值 u16
    QList<quint8> m_record_Battery_bal;//记录电池信息的均衡值

    QList<quint8> m_record_Temp_Index; //记录温度信息的index
    QList<QString> m_record_Temp_Time; //记录温度信息的time
    QList<qint16> m_record_Temp_v;//记录温度信息的温度值 s16

    QList<QVariant> m_record_Other;  //记录其他信息

    QList<quint8> m_record_Other_Index; //记录其他信息的index
    QList<QString> m_record_Other_Time; //记录其他信息的time
    QList<QVariant> m_storage_Other;  //储存其他信息

    //--------------------



    QList<quint32> m_recordBatteryCnt;//已经记录到的序号，导出后全部清4
    QList<quint32> m_recordTempCnt;//已经记录到的序号，导出后全部清4
    QList<quint32> m_recordOtherCnt;//已经记录到的序号，导出后全部清4


    //-----------通讯失败处理---------------
    quint32 m_comFailCnt;  //通讯失败的次数 默认20
    bool m_continueCom;//是否继续通讯  默认false

    //---------记录间隔
    quint32 m_record_Rx_Scale;  //此值为记录与扫描的次数比，默认是扫描一次，记录一次

    quint16 m_rx_cnt; //接收到数据的次数

    //--------------------校准温度选项
    QList<bool> m_correctTempIndex;  //校准温度的index是否校准
    QList<qint16> m_correctTempValue;  //校准的温度值
    //bool isComSleep;
};

#endif // BMS_H
