#include "loginfomodel.h"
#include <algorithm>
#include <cmath>
#include <QDebug>
LogInfo::LogInfo(QString eventType,QDateTime date,double totalV,double totalI,qint8 temp,
                 quint32 cap,quint32 leftCap,QByteArray bmsStatus,bool deleteLog):
    m_eventType(eventType),m_date(date),m_totalV(totalV),m_totalI(totalI),m_temp(temp),
    m_cap(cap),m_leftCap(leftCap),m_bmsStatus(bmsStatus),m_deleteLog(deleteLog)
{

}

QString LogInfo::eventType() const
{
    return m_eventType;
}

void LogInfo::setEventType(const QString &eventType)
{
    m_eventType = eventType;
}

QDateTime LogInfo::date() const
{
    return m_date;
}

void LogInfo::setDate(const QDateTime &date)
{
    m_date = date;
}

double LogInfo::totalV() const
{
    return m_totalV;
}

void LogInfo::setTotalV(double totalV)
{
    m_totalV = totalV;
}

double LogInfo::totalI() const
{
    return m_totalI;
}

void LogInfo::setTotalI(double totalI)
{
    m_totalI = totalI;
}

qint8 LogInfo::temp() const
{
    return m_temp;
}

void LogInfo::setTemp(const qint8 &temp)
{
    m_temp = temp;
}

quint32 LogInfo::cap() const
{
    return m_cap;
}

void LogInfo::setCap(const quint32 &cap)
{
    m_cap = cap;
}

quint32 LogInfo::leftCap() const
{
    return m_leftCap;
}

void LogInfo::setLeftCap(const quint32 &leftCap)
{
    m_leftCap = leftCap;
}

QByteArray LogInfo::bmsStatus() const
{
    return m_bmsStatus;
}

void LogInfo::setBmsStatus(const QByteArray &bmsStatus)
{
    m_bmsStatus = bmsStatus;
}

bool LogInfo::deleteLog() const
{
    return m_deleteLog;
}

void LogInfo::setDeleteLog(bool deleteLog)
{
    m_deleteLog = deleteLog;
}




//------------------------------------------------------

LogInfoModel::LogInfoModel(QObject *parent) :QAbstractListModel(parent),
    m_startYear(0),m_stopYear(0)
{

}

void LogInfoModel::addAllLogInfo(QList<LogInfo>& logInfoList)
{

    for(int i=0;i<logInfoList.size();i++)
        m_logInfo<<logInfoList[i];

    if(m_logInfo.size()<11){
        for(int i=0;i<m_logInfo.size();i++)
            addShowLogInfo(m_logInfo[i]);
    }
}
void LogInfoModel::clear()
{
    m_logInfo.clear();
    m_filterLogInfo.clear();
    m_filterIndex.clear();
    showLogInfoClear();
}

int LogInfoModel::getStartYear()
{
    m_startYear=m_logInfo[0].date().date().year();
    qDebug()<<m_startYear;
    return m_startYear;
}

int LogInfoModel::getStopYear()
{
    m_stopYear=m_logInfo[m_logInfo.size()-1].date().date().year();
    return m_stopYear;
}

int LogInfoModel::getNeedLog(QString startTime, QString stopTime)  //还要记录下对应的原m_logInfo中的index
{
    m_filterLogInfo.clear();
    m_filterIndex.clear();
    showLogInfoClear();
    qDebug()<<"getNeedLog";
    qDebug()<<startTime<<stopTime;
    QDateTime d1=QDateTime::fromString(startTime,"yyyy'-'M'-'d' 'H:m:s");
    QDateTime d2=QDateTime::fromString(stopTime,"yyyy'-'M'-'d' 'H:m:s");
    qDebug()<<d1<<d2;
    QList<LogInfo> x;
    quint32 j=0;
    for(int i=0;i<m_logInfo.size();i++){
        if(m_logInfo[i].date()>=d1 && m_logInfo[i].date()<=d2){
            j++;
            x<<m_logInfo[i];   //为了防止出现得到的太多而响应慢的问题才如此转一次
            m_filterIndex<<i;
            if(j%10==0){
                addFilterLogInfo(x);
                x.clear();
                emit readLogOnePage();
            }
        }
    }
    if(!(j%10==0)){
        addFilterLogInfo(x);
        emit readLogOnePage();
    }
    emit showLogCnt(j);
    return 0;
}

int LogInfoModel::getRxLog()
{
    showLogInfoClear();
    if(m_logInfo.size()<11){
        for(int i=0;i<m_logInfo.size();i++)
            addShowLogInfo(m_logInfo[i]);
    }
    else {
        for(int i=0;i<10;i++)
            addShowLogInfo(m_logInfo[i]);
    }
    emit showLogCnt(m_logInfo.size());
    emit readLogTotalPage(std::ceil((double)m_logInfo.size()/10));
    return 0;
}

void LogInfoModel::recordDeleteLog(int rowNo, int pageNo, bool status,int showFlag)//pageFlag 在筛选页按下还是显示页按下
{
    qDebug()<<"recordDeleteLog";
    if(0==showFlag){
        //在显示页按下
        m_logInfo[rowNo+pageNo*10].setDeleteLog(status);
    }
    else if(1==showFlag){
        m_filterLogInfo[rowNo+pageNo*10].setDeleteLog(status);
        m_logInfo[m_filterIndex[rowNo+pageNo*10]].setDeleteLog(status);
    }
//    qDebug()<<"----------recordDeleteLog------------";
//    for(int i=0;i<m_logInfo.size();i++){
//        qDebug()<<m_logInfo[i].deleteLog();
//    }

}

void LogInfoModel::showDeleteLog()
{
    qDebug()<<"---------showDeleteLog--------------";
    QMap<QList<int>, bool>::const_iterator i = m_recordDeleteLog.constBegin();
    while (i != m_recordDeleteLog.constEnd()) {
        qDebug() << i.key()[0]<<i.key()[1] << ": " << i.value() << endl;
        ++i;
    }
}

void LogInfoModel::deleteSelect(int pageNo,int showFlag)
{
    qDebug()<<"deleteSelect"<<pageNo<<showFlag;

    for (int i=0;i<m_logInfo.size();) {
        if(m_logInfo[i].deleteLog()){
            m_logInfo.removeAt(i);
        }
        else {
            i++;
        }
    }
    for (int i=0;i<m_filterLogInfo.size();) {
        if(m_filterLogInfo[i].deleteLog()){
            m_filterLogInfo.removeAt(i);
        }
        else {
            i++;
        }
    }

    //更新条数，页数
    if(0==showFlag){
        emit  readLogTotalPage(std::ceil((double)m_logInfo.size()/10));//日志的总页数
        emit  showLogCnt(m_logInfo.size());
    }
    else if(1==showFlag){
        emit  readLogTotalPage(std::ceil((double)m_filterLogInfo.size()/10));//日志的总页数
        emit  showLogCnt(m_filterLogInfo.size());
    }

    getPage(pageNo,showFlag);
}

void LogInfoModel::deleteAll(int pageNo,int showFlag)
{
    qDebug()<<"deleteAll";
    if(0==showFlag){
        //在显示页按下
        for (int i=0;i<m_logInfo.size();i++) {
            m_logInfo[i].setDeleteLog(true);
        }

    }
    else if(1==showFlag){
        for (int i=0;i<m_filterLogInfo.size();i++) {
            m_filterLogInfo[i].setDeleteLog(true);
        }
        for (int i=0;i<m_filterIndex.size();i++) {
            m_logInfo[m_filterIndex[i]].setDeleteLog(true);
        }

    }

    deleteSelect(pageNo,showFlag);
}

bool LogInfoModel::getDeleteLog(int rowNo, int pageNo, int showFlag)
{
    if(0==showFlag)
        return m_logInfo[rowNo+pageNo*10].deleteLog();
    else {
        return m_filterLogInfo[rowNo+pageNo*10].deleteLog();
    }
}
void LogInfoModel::showLogInfoClear()
{
    emit beginResetModel();
    m_showLogInfo.clear();
    emit endResetModel();

}

void LogInfoModel::addFilterLogInfo(QList<LogInfo> & filterLogInfo)
{

    for(int i=0;i<filterLogInfo.size();i++)
        m_filterLogInfo<<filterLogInfo[i];
    if(m_filterLogInfo.size()<11){
        for(int i=0;i<m_filterLogInfo.size();i++)
            addShowLogInfo(m_filterLogInfo[i]);
    }
}


void LogInfoModel::getPage(int pageNo,int showFlag) //showFlag 为0，显示的是所有的数据  为1，显示的是筛选的数据
{
    qDebug()<<"getPage"<<pageNo;
    showLogInfoClear();
    if(0==showFlag){//读所有日志
        if(m_logInfo.size()<((pageNo+1)*10)){
            //读最后一页的内容
            for(int i=pageNo*10;i<m_logInfo.size();i++)
                addShowLogInfo(m_logInfo[i]);
        }
        else {
            for(int i=pageNo*10;i<(pageNo*10+10);i++)
                addShowLogInfo(m_logInfo[i]);
        }
    }
    else if(1==showFlag){//读筛选日志
        if(m_filterLogInfo.size()<((pageNo+1)*10)){
            //读最后一页的内容
            for(int i=pageNo*10;i<m_filterLogInfo.size();i++)
                addShowLogInfo(m_filterLogInfo[i]);
        }
        else {
            for(int i=pageNo*10;i<(pageNo*10+10);i++)
                addShowLogInfo(m_filterLogInfo[i]);
        }
    }
    qDebug()<<"m_showlogInfo.size()"<<m_showLogInfo.size();

}

void LogInfoModel::clearShow()
{
    //清除显示
    showLogInfoClear();
}


void LogInfoModel::addShowLogInfo(LogInfo& logInfo)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_showLogInfo<<logInfo;
    endInsertRows();

}
int LogInfoModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_showLogInfo.count();
}

QVariant LogInfoModel::data(const QModelIndex & index, int role) const {


    if (index.row() < 0 || index.row() >= m_showLogInfo.count())
        return QVariant();

    const LogInfo &logInfo = m_showLogInfo[index.row()];
    if (role == deleteRole){
         qDebug()<<"data1"<<logInfo.deleteLog();
            return logInfo.deleteLog();

        }
    else if (role == eventTypeRole){
        return logInfo.eventType();
    }
    else if (role == dateRole){
        return logInfo.date().toString("yyyy-MM-dd HH:mm:ss");
    }
    else if (role == totalVRole){
        return logInfo.totalV();
    }
    else if (role == totalIRole){
        return logInfo.totalI();
    }
    else if (role == tempRole){
        return logInfo.temp();
    }
    else if (role == capRole){
        return logInfo.cap();
    }
    else if (role == leftCapRole){
        return logInfo.leftCap();
    }
    else if (role == bmsStatusRole){
        return logInfo.bmsStatus();
    }

    return QVariant();
}


QHash<int, QByteArray> LogInfoModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[deleteRole] = "deleteRole";
    roles[eventTypeRole] = "eventTypeRole";
    roles[dateRole] = "dateRole";
    roles[totalVRole] = "totalVRole";
    roles[totalIRole] = "totalIRole";
    roles[tempRole] = "tempRole";
    roles[capRole] = "capRole";
    roles[leftCapRole] = "leftCapRole";
    roles[bmsStatusRole] = "bmsStatusRole";

    return roles;
}

