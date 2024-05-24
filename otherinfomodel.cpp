#include "otherinfomodel.h"

OtherInfo::OtherInfo(QString name,QString unit):
    m_name(name),m_v(0),m_unit(unit),m_record(true),m_scan(true)
{

}

QString OtherInfo::name() const
{
    return m_name;
}

void OtherInfo::setName(const QString &name)
{
    m_name = name;
}

double OtherInfo::v() const
{
    return m_v;
}

void OtherInfo::setV(double v)
{
    m_v = v;
}

QString OtherInfo::unit() const
{
    return m_unit;
}

void OtherInfo::setUnit(const QString &unit)
{
    m_unit = unit;
}

bool OtherInfo::record() const
{
    return m_record;
}

void OtherInfo::setRecord(bool record)
{
    m_record = record;
}

bool OtherInfo::scan() const
{
    return m_scan;
}

void OtherInfo::setScan(bool scan)
{
    m_scan = scan;
}


//---------------------------model
//------------------------------------------------------

OtherInfoModel::OtherInfoModel(QObject *parent) :QAbstractTableModel(parent)
{

}

void OtherInfoModel::clear()
{
    emit beginResetModel();
    m_otherInfo.clear();
    emit endResetModel();

}

void OtherInfoModel::addOtherInfo(const QString &name,const QString& unit)
{
    //qDebug()<<"addBatteryInfo";
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_otherInfo << OtherInfo(name,unit);
    endInsertRows();

}

int OtherInfoModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_otherInfo.count();
}

int OtherInfoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant OtherInfoModel::data(const QModelIndex & index, int role) const {
    //qDebug()<<"--------data-------";
    if (index.row() < 0 || index.row() >= m_otherInfo.count())
        return QVariant();

    const OtherInfo &otherInfo = m_otherInfo[index.row()];
    if (role == nameRole){
        //qDebug()<<batteryInfo.getName();
        return otherInfo.name();
    }
    else if (role == vRole){
        return QString::number(otherInfo.v());
    }
    else if (role==unitRole) {
        return otherInfo.unit();
    }
    else if (role==recordRole) {
        return otherInfo.record();
    }
    else if (role==scanRole) {
        return otherInfo.scan();
    }
    return QVariant();
}

bool OtherInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= m_otherInfo.count())
        return false;
    OtherInfo& otherInfo = m_otherInfo[index.row()];
    if (role == nameRole){
        otherInfo.setName(value.toString());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if (role == vRole){
        otherInfo.setV(value.toDouble());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if (role == unitRole){
        otherInfo.setUnit(value.toString());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if(role==recordRole){
        otherInfo.setRecord(value.toBool());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    else if(role==scanRole){
        otherInfo.setScan(value.toBool());
        emit dataChanged(index,index,QVector<int>(1,role));
        return true;
    }
    return false;
}

QModelIndex OtherInfoModel::getIndex(int row, int col)
{
    return createIndex(row,col);
}

void OtherInfoModel::allRecord(bool value)
{
    for (int i=0;i<m_otherInfo.size();i++) {
        setData(getIndex(i,2), value, recordRole);
    }

}

void OtherInfoModel::record(int index, bool value)
{
    setData(getIndex(index,3), value, recordRole);
    if(value){
        int i=0;
        for (;i<m_otherInfo.size();i++) {
            if(false==m_otherInfo[i].record()){
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

void OtherInfoModel::allScan(bool value)
{
    for (int i=0;i<m_otherInfo.size();i++) {
        setData(getIndex(i,3), value, scanRole);
    }
}

void OtherInfoModel::scan(int index, bool value)
{
    setData(getIndex(index,4), value, scanRole);
    if(value){
        int i=0;
        for (;i<m_otherInfo.size();i++) {
            if(false==m_otherInfo[i].scan()){
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

bool OtherInfoModel::getRecord(int index)
{
    return m_otherInfo[index].record();
}

bool OtherInfoModel::getScan(int index)
{
    return m_otherInfo[index].scan();
}


QHash<int, QByteArray> OtherInfoModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[nameRole] = "nameRole";
    roles[vRole] = "vRole";
    roles[unitRole]="unitRole";
    roles[recordRole] = "recordRole";
    roles[scanRole] = "scanRole";
    return roles;
}

