#ifndef LOGINFOMODEL_H
#define LOGINFOMODEL_H
#include <QAbstractListModel>
#include <QObject>
#include <QDateTime>
class LogInfo
{
public:
    LogInfo(QString eventType,QDateTime date,double totalV,double totalI,qint8 temp,
            quint32 cap,quint32 leftCap,QByteArray bmsStatus,bool deleteLog=false);



    QString eventType() const;
    void setEventType(const QString &eventType);

    QDateTime date() const;
    void setDate(const QDateTime &date);

    double totalV() const;
    void setTotalV(double totalV);

    double totalI() const;
    void setTotalI(double totalI);

    qint8 temp() const;
    void setTemp(const qint8 &temp);

    quint32 cap() const;
    void setCap(const quint32 &cap);

    quint32 leftCap() const;
    void setLeftCap(const quint32 &leftCap);

    QByteArray bmsStatus() const;
    void setBmsStatus(const QByteArray &bmsStatus);



    bool deleteLog() const;
    void setDeleteLog(bool deleteLog);

private:
    QString m_eventType;
    QDateTime m_date;
    double m_totalV;
    double m_totalI;
    qint8 m_temp;
    quint32 m_cap;
    quint32 m_leftCap;
    QByteArray m_bmsStatus;
    bool m_deleteLog;
};


class LogInfoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum LogInfoRoles {
        deleteRole= Qt::UserRole + 1,
        eventTypeRole,
        dateRole,
        totalVRole,
        totalIRole,
        tempRole,
        capRole,
        leftCapRole,
        bmsStatusRole

    };

    explicit LogInfoModel(QObject *parent = 0);
    //Q_INVOKABLE void addLogInfo(QString eventType,QString date,double totalV,double totalI,qint8 temp,
    //quint32 cap,quint32 leftCap,QByteArray bmsStatus);
    Q_INVOKABLE void addAllLogInfo(QList<LogInfo>& logInfoList);
    Q_INVOKABLE void addShowLogInfo(LogInfo& logInfo);

    Q_INVOKABLE void clear();

    Q_INVOKABLE int getStartYear();
    Q_INVOKABLE int getStopYear();

    Q_INVOKABLE int getNeedLog(QString startTime,QString stopTime);
    Q_INVOKABLE int getRxLog();

    Q_INVOKABLE void recordDeleteLog(int rowNo,int pageNo,bool status,int showFlag);//记录要删除的日志，pageFlag：在筛选页按下还是显示页按下
    Q_INVOKABLE void showDeleteLog();

    Q_INVOKABLE void deleteSelect(int pageNo,int showFlag);  //删除选中的日志
    Q_INVOKABLE void deleteAll(int pageNo,int showFlag);  //删除全部日志，showFlag区分是qu全部还是筛选的

    Q_INVOKABLE bool getDeleteLog(int rowNo,int pageNo,int showFlag);


    void showLogInfoClear();
    void addFilterLogInfo(QList<LogInfo>&);



    Q_INVOKABLE void getPage(int pageNo,int showFlag);//得到那一页的数据  //showFlag 为0，显示的是所有的数据  为1，显示的是筛选的数据

    Q_INVOKABLE void clearShow();//清除显示

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
signals:
    void  readLogOnePage();
    void  readLogTotalPage(quint32 cnt);//日志的总页数
    void  showLogCnt(quint32 cnt);

public slots:
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<LogInfo> m_logInfo;
    QList<LogInfo> m_filterLogInfo;
    QList<int> m_filterIndex;//筛选的日志对应的m_logInfo中的索引值，以方便删除
    QList<LogInfo> m_showLogInfo;
    QList<int> m_showLogInfoDelete;

    QMap<QList<int>,bool> m_recordDeleteLog;//bool 按下为true  未按下为false

    int m_startYear;
    int m_stopYear;

};

#endif // LOGINFOMODEL_H
