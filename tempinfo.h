#ifndef TEMPINFO_H
#define TEMPINFO_H

#include <QObject>
#include <QAbstractTableModel>
class TempInfo
{
public:
    TempInfo(QString s,qint16 v);
    QString name() const;
    void setName(const QString &name);

    qint16 temp() const;
    void setTemp(const qint16 &v);

    bool record() const;
    void setRecord(bool record);

    bool scan() const;
    void setScan(bool scan);

private:
    QString m_name;
    qint16 m_temp;
    bool m_record;
    bool m_scan;
};


class TempInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum TempInfoRoles {
        nameRole = Qt::UserRole + 1,
        tempRole,
        recordRole,
        scanRole
     };

    explicit TempInfoModel(QObject *parent = 0);
    Q_INVOKABLE void addTempInfo(const QString &name,const qint16& temp);
    Q_INVOKABLE void addAll();
    Q_INVOKABLE void clear();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
int columnCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QModelIndex getIndex(int row,int col);

    //----------------------全记录
     Q_INVOKABLE void allRecord(bool value);
    //----------------------分记录
    Q_INVOKABLE void record(int index,bool value);

    //----------------------全扫描
    Q_INVOKABLE void allScan(bool value);
    //----------------------分扫描
    Q_INVOKABLE void scan(int index,bool value);

    //----------------是否记录和扫描
    bool getRecord(int index);
    bool getScan(int index);

signals:
    void isAllRecord(bool value);
    void isAllScan(bool value);

public slots:
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<TempInfo> m_tempInfo;

};

#endif // TEMPINFO_H
