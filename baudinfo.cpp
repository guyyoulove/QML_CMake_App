#include "baudinfo.h"
#include <QDebug>
BaudInfo::BaudInfo(QObject *parent) : QAbstractListModel(parent),m_indexOf115(0)
{

}
void BaudInfo::addAll()
{
    //qDebug()<<"-------------baudInfo-----------";
    clear();
    QList<qint32> baudList=QSerialPortInfo::standardBaudRates();
    int cnt=0;
    for(int i=0;i<baudList.size();i++){
        if(115200==baudList[i]){
            m_indexOf115=i-cnt;
        }
        if(baudList[i]<4800){
            cnt++;
        }
        else {
            addBaud(baudList[i]);
        }

    }
}
void BaudInfo::clear()
{
    emit beginResetModel();
    m_baudInfo.clear();
    emit endResetModel();

}

int BaudInfo::getIndexOf115()
{
    return m_indexOf115;
}

void BaudInfo::addBaud(qint32 &baud)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_baudInfo << baud;
    endInsertRows();
}



int BaudInfo::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_baudInfo.count();
}

QVariant BaudInfo::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_baudInfo.count())
        return QVariant();

    const unsigned int &i = m_baudInfo[index.row()];
    if (role == baudRole)
        return i;
    return QVariant();
}


QHash<int, QByteArray> BaudInfo::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[baudRole] = "baudRole";
    return roles;
}

