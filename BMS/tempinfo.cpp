#include "tempinfo.h"
#include <QDebug>
TempInfo::TempInfo(QString s,qint16 temp):m_name(s),m_temp(temp),m_record(true),m_scan(true)
{

}

QString TempInfo::name() const
{
    return m_name;
}

void TempInfo::setName(const QString &name)
{
    m_name = name;
}

qint16 TempInfo::temp() const
{
    return m_temp;
}

void TempInfo::setTemp(const qint16 &temp)
{
    m_temp = temp;
}

bool TempInfo::record() const
{
    return m_record;
}

void TempInfo::setRecord(bool record)
{
    m_record = record;
}

bool TempInfo::scan() const
{
    return m_scan;
}

void TempInfo::setScan(bool scan)
{
    m_scan = scan;
}

//------------------------------------------------------

TempInfoModel::TempInfoModel(QObject *parent) :QAbstractTableModel(parent)
{

}

void TempInfoModel::addAll()
{
    //   for(int i=0;i<10;i++)
    //       addBatteryInfo("c");
    //qDebug()<<"-------显示m_batteryInfo中的内容-----";
    //qDebug()<<m_batteryInfo.size();
    //for(int i=0;i<m_batteryInfo.size();i++)
    //qDebug()<<m_batteryInfo[i].getName();
}
void TempInfoModel::clear()
{
    emit beginResetModel();
    m_tempInfo.clear();
    emit endResetModel();

}

void TempInfoModel::addTempInfo(const QString &name,const qint16& temp)
{
    //qDebug()<<"addBatteryInfo";
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_tempInfo << TempInfo(name,temp);
    endInsertRows();

}

int TempInfoModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_tempInfo.count();
}

int TempInfoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant TempInfoModel::data(const QModelIndex & index, int role) const {
    //qDebug()<<"--------data-------";
    if (index.row() < 0 || index.row() >= m_tempInfo.count())
        return QVariant();

    const TempInfo &tempInfo = m_tempInfo[index.row()];
    if (role == nameRole){
        //qDebug()<<batteryInfo.getName();
        return tempInfo.name();
    }
    else if (role == tempRole){
        return QString::number(tempInfo.temp());
    }
    else if (role==recordRole) {
        return tempInfo.record();
    }
    else if (role==scanRole) {
        return tempInfo.scan();
    }
    return QVariant();
}

bool TempInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= m_tempInfo.count())
        return false;
    TempInfo& tempInfo = m_tempInfo[index.row()];
    if (role == nameRole){
        tempInfo.setName(value.toString());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if (role == tempRole){
        tempInfo.setTemp(value.toInt());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }

    else if(role==recordRole){
        qDebug()<<"TempInfoModel::setData"<<index<<value;
        tempInfo.setRecord(value.toBool());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if(role==scanRole){
        tempInfo.setScan(value.toBool());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    return false;
}

QModelIndex TempInfoModel::getIndex(int row, int col)
{
    return createIndex(row,col);
}

void TempInfoModel::allRecord(bool value)
{
    for (int i=0;i<m_tempInfo.size();i++) {
        setData(getIndex(i,2), value, recordRole);
    }

}

void TempInfoModel::record(int index, bool value)
{

    setData(getIndex(index,2), value, recordRole);
    if(value){
        int i=0;
        for (;i<m_tempInfo.size();i++) {
            if(false==m_tempInfo[i].record()){
                //不是全选
                emit isAllRecord(false);
                return;
            }
        }
        emit isAllRecord(true);
    }
    else {
        emit isAllRecord(false);
    }

}

void TempInfoModel::allScan(bool value)
{
    for (int i=0;i<m_tempInfo.size();i++) {
        setData(getIndex(i,3), value, scanRole);
    }
}

void TempInfoModel::scan(int index, bool value)
{
    setData(getIndex(index,3), value, scanRole);
    if(value){
        int i=0;
        for (;i<m_tempInfo.size();i++) {
            if(false==m_tempInfo[i].scan()){
                //不是全选
                emit isAllScan(false);
                return;
            }
        }
        emit isAllScan(true);
    }
    else {
        emit isAllScan(false);
    }
}

bool TempInfoModel::getRecord(int index)
{
    return m_tempInfo[index].record();
}

bool TempInfoModel::getScan(int index)
{
    return m_tempInfo[index].scan();
}


QHash<int, QByteArray> TempInfoModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[nameRole] = "nameRole";
    roles[tempRole] = "tempRole";
    roles[recordRole] = "recordRole";
    roles[scanRole] = "scanRole";
    return roles;
}

