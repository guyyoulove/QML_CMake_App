#ifndef SERIALPORTINFO_H
#define SERIALPORTINFO_H

#include <QObject>
#include <QSerialPortInfo>
#include <QList>
#include <QString>
#include <QAbstractListModel>



class SerialPortInfo : public QAbstractListModel
{
    Q_OBJECT
public:
    enum SerialPortInfoRoles {
        portRole = Qt::UserRole + 1,
    };

    SerialPortInfo(QObject *parent = 0);


    Q_INVOKABLE void addSerialPort(const QString &serialPortName);
    Q_INVOKABLE void addAll();
    Q_INVOKABLE void update();
    Q_INVOKABLE void clear();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<QString> m_serialPortInfo;


};
#endif // SERIALPORTINFO_H
