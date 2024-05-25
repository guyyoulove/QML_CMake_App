#ifndef COMTHREAD_H
#define COMTHREAD_H
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSerialPort>
#include <QTimer>
class ComThread : public QThread
{
    Q_OBJECT
public:
    ComThread(QObject *parent=0);
    ~ComThread();
    void transaction(const QString &portName,const uint &baudRate,const int& dataBits,
                     const int &stopBits,const QString &parity,QList<QByteArray>& commandList,bool isCycle,quint8 comFlag);
    void closeSerialPort();
    void run() override;

    void stopReadInfo();

    void sendChanged(QList<QByteArray>& commandList,bool isCycle,quint8 comFlag,quint8 batteryCnt);//comFlag为通讯标志

    void changeDelay(quint16 waitRx_MS,quint16 waitRx_byte_MS,quint16 rxFailWait_MS,quint16 rxWait_MS);

   //--------------------通讯失败处理
    void setComFailAction(quint32& comFailCnt,bool continueCom);

    //------------------唤醒处理
    void wakeUp();


signals:
    void responseData(QByteArray dataArray);
    void responseMsg(const QString &s,bool isDialog);//isDialog:true 对话框提示  false:comstatus提示
    void error(const QString &s);
    void responseTime(QString time);
protected:
private slots:
    void tenSecShot();
private:
    QByteArray calCheck(QByteArray& waitCheck);
    bool m_quit;
    QMutex m_mutex;
    QWaitCondition m_cond;
    //----------------------------
    QString m_portName;
    uint m_baudRate;
    int m_dataBits;
    int m_stopBits;
    QSerialPort::Parity m_parity;
    uint m_waitTimeOut;

    QList<QByteArray> m_commandList;//发送的数据帧
    bool m_isCycle;
    bool m_pause;
    bool m_newData; //有新的发送数据来


    quint8 m_comFlag;
    quint8 m_batteryCnt;

    quint16 m_waitRx_MS;
    quint16 m_waitRx_byte_MS;
    quint16 m_rxFailWait_MS;
    quint16 m_rxWait_MS;

    //通讯失败处理
    quint32 m_comFailCnt;
    bool m_continueCom;

    bool m_comSleep;  //通讯暂停了，要求每隔十秒发一次唤醒帧，发十次，间隔50ms

     QTimer timer ;
};

#endif // COMTHREAD_H
