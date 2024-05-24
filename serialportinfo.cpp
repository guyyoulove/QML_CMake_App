#include "serialportinfo.h"
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
SerialPortInfo::SerialPortInfo(QObject *parent) :QAbstractListModel(parent)
{

}

void SerialPortInfo::addAll()
{
    clear();
    QList<QSerialPortInfo> portList=QSerialPortInfo::availablePorts();
    for(auto& a:portList){
        addSerialPort(a.portName());
    }
}

void SerialPortInfo::update()
{
    QList<QSerialPortInfo> portList=QSerialPortInfo::availablePorts();
    QList<QString> newPortNameList;
    for (int i=0;i<portList.size();i++) {
        newPortNameList<<portList[i].portName();
    }

    for (int i=0;i<m_serialPortInfo.size();i++) { //检查已经失效的串口
        if(!newPortNameList.contains(m_serialPortInfo[i])){
            //删除该串口
            beginRemoveRows(QModelIndex(), i, i);
            m_serialPortInfo.removeAt(i);
            endRemoveRows();
            break;
        }
    }

    for (int i=0;i<newPortNameList.size();i++) { //检查新增的串口
        if(!m_serialPortInfo.contains(newPortNameList[i])){
            //增加该串口
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_serialPortInfo<<newPortNameList[i];
            endInsertRows();
            break;
        }
    }

}
void SerialPortInfo::clear()
{
    emit beginResetModel();
    m_serialPortInfo.clear();
    emit endResetModel();

}

void SerialPortInfo::addSerialPort(const QString &serialPort)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_serialPortInfo << serialPort;
    endInsertRows();
}

int SerialPortInfo::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_serialPortInfo.count();
}

QVariant SerialPortInfo::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_serialPortInfo.count())
        return QVariant();

    const QString &serialPortName = m_serialPortInfo[index.row()];
    if (role == portRole)
        return serialPortName;
    return QVariant();
}


QHash<int, QByteArray> SerialPortInfo::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[portRole] = "portRole";
    return roles;
}



