#include "batteryinfo.h"
#include <QDebug>
#include <QVector>

BatteryInfoModel::BatteryInfoModel(QObject *parent) :QAbstractTableModel(parent)
{

}

void BatteryInfoModel::clear()
{
    emit beginResetModel();
    m_batteryInfo.clear();
    emit endResetModel();

}

void BatteryInfoModel::addBatteryInfo(const QString &name,quint16 v,bool bal)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_batteryInfo << BatteryInfo(name,v,bal);
    endInsertRows();

}



int BatteryInfoModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_batteryInfo.count();
}
int BatteryInfoModel::columnCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return 5;
}

QVariant BatteryInfoModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_batteryInfo.count())
        return QVariant();

    const BatteryInfo &batteryInfo = m_batteryInfo[index.row()];
    if (role == nameRole){
        return batteryInfo.getName();
    }
    else if (role == vRole){
        return QString::number(batteryInfo.getV());
    }
    else if(role==balRole){
        return batteryInfo.getBal();
    }
    else if(role==recordRole){
        return batteryInfo.getRecord();
    }
    else if(role==scanRole){
        return batteryInfo.getScan();
    }
    return QVariant();
}

bool BatteryInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= m_batteryInfo.count())
        return false;
    BatteryInfo& batteryInfo = m_batteryInfo[index.row()];
    if (role == nameRole){
        batteryInfo.setName(value.toString());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if (role == vRole){
        batteryInfo.setV(value.toUInt());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if(role==balRole){
        batteryInfo.setBal(value.toBool());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if(role==recordRole){
        batteryInfo.setRecord(value.toBool());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if(role==scanRole){
        batteryInfo.setScan(value.toBool());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    return false;
}


QModelIndex BatteryInfoModel::getIndex(int row, int col)
{
    return createIndex(row,col);
}

void BatteryInfoModel::allRecord(bool value)
{
    for (int i=0;i<m_batteryInfo.size();i++) {
        setData(getIndex(i,3), value, recordRole);
    }

}

void BatteryInfoModel::record(int index, bool value)
{
    setData(getIndex(index,3), value, recordRole);
    if(value){
        int i=0;
        for (;i<m_batteryInfo.size();i++) {
            if(false==m_batteryInfo[i].getRecord()){
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

void BatteryInfoModel::allScan(bool value)
{
    for (int i=0;i<m_batteryInfo.size();i++) {
        setData(getIndex(i,4), value, scanRole);
    }
}

void BatteryInfoModel::scan(int index, bool value)
{
    setData(getIndex(index,4), value, scanRole);
    if(value){
        int i=0;
        for (;i<m_batteryInfo.size();i++) {
            if(false==m_batteryInfo[i].getScan()){
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

bool BatteryInfoModel::getRecord(int index)
{
    return m_batteryInfo[index].getRecord();
}

bool BatteryInfoModel::getScan(int index)
{
    return m_batteryInfo[index].getScan();
}



QHash<int, QByteArray> BatteryInfoModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[nameRole] = "nameRole";
    roles[vRole] = "vRole";
    roles[balRole] ="balRole";
    roles[recordRole]="recordRole";
    roles[scanRole]="scanRole";
    return roles;
}


//------------------------------------------------------------
BatteryInfo::BatteryInfo(QString s,quint16 v,bool bal):m_name(s),
    m_v(v),m_bal(bal),m_scan(true),m_record(true)
{

}

QString BatteryInfo::getName() const
{
    return m_name;
}

void BatteryInfo::setName(const QString &value)
{
    m_name = value;
}

quint16 BatteryInfo::getV() const
{
    return m_v;
}

void BatteryInfo::setV(quint16 value)
{
    m_v = value;
}

bool BatteryInfo::getBal() const
{
    return m_bal;
}

void BatteryInfo::setBal(bool bal)
{
    m_bal = bal;
}

bool BatteryInfo::getScan() const
{
    return m_scan;
}

void BatteryInfo::setScan(bool scan)
{
    m_scan = scan;
}

bool BatteryInfo::getRecord() const
{
    return m_record;
}

void BatteryInfo::setRecord(bool record)
{
    m_record = record;
}
