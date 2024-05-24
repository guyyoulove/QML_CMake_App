#ifndef BAUDINFO_H
#define BAUDINFO_H

#include <QObject>
#include <QSerialPortInfo>
#include <QList>
#include <QString>
#include <QAbstractListModel>
#include "serialportinfo.h"

class BaudInfo : public QAbstractListModel
{
    Q_OBJECT
public:
    enum BaudInfoRoles {
        baudRole = Qt::UserRole + 1,
    };

    BaudInfo(QObject *parent = 0);

    Q_INVOKABLE void addBaud(qint32 &baud);
    Q_INVOKABLE void addAll();
    Q_INVOKABLE void clear();
    Q_INVOKABLE int getIndexOf115(); //得到波特率115200的index(为使用方便，直接就显示9600)
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<qint32> m_baudInfo;
    quint8 m_indexOf115;

};

#endif // BAUDINFO_H
