#ifndef BATTERYINFO_H
#define BATTERYINFO_H

#include <QObject>
#include <QAbstractTableModel>
class BatteryInfo{
public:
    BatteryInfo(QString s,quint16 v,bool bal);

    QString getName() const;
    void setName(const QString &value);

    quint16 getV() const;
    void setV(quint16 value);

    bool getBal() const;
    void setBal(bool bal);

    bool getScan() const;
    void setScan(bool scan);

    bool getRecord() const;
    void setRecord(bool record);

private:
    QString m_name;
    quint16 m_v;
    bool m_bal;
    bool m_scan; //扫描
    bool m_record; //记录

};

class BatteryInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum BatteryInfoRoles {
        nameRole = Qt::UserRole + 1,
        vRole,
        balRole,
        recordRole,
        scanRole
     };

    explicit BatteryInfoModel(QObject *parent = 0);
    Q_INVOKABLE void addBatteryInfo(const QString &name,const quint16 v,bool bal);
    Q_INVOKABLE void clear();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) ;

    QModelIndex getIndex(int row,int col);

    //----------------------全记录
     Q_INVOKABLE void allRecord(bool value);
    //----------------------分记录
     Q_INVOKABLE void record(int index,bool value);

    //----------------------全扫描
    Q_INVOKABLE void allScan(bool value);
    //----------------------分扫描
    Q_INVOKABLE void scan(int index,bool value);

    //--------------是否记录和扫描
    bool getRecord(int index);
    bool getScan(int index);



signals:
    void isAllRecord(bool value);
    void isAllScan(bool value);

public slots:
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<BatteryInfo> m_batteryInfo;

};

#endif // BATTERYINFO_H
