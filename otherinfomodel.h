#ifndef OTHERINFOMODEL_H
#define OTHERINFOMODEL_H

#include <QObject>
#include <QAbstractTableModel>
class OtherInfo{
public:
    OtherInfo(QString name,QString unit);

    QString name() const;
    void setName(const QString &name);

    double v() const;
    void setV(double v);

    QString unit() const;
    void setUnit(const QString &unit);

    bool record() const;
    void setRecord(bool record);

    bool scan() const;
    void setScan(bool scan);

private:
    QString m_name;
    double  m_v;
    QString m_unit;
    bool m_record; //记录
    bool m_scan; //扫描


};

class OtherInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum OtherInfoRoles {
        nameRole = Qt::UserRole + 1,
        vRole,
        unitRole,
        recordRole,
        scanRole
     };

    explicit OtherInfoModel(QObject *parent = 0);
    Q_INVOKABLE void addOtherInfo(const QString &name,const QString &unit);
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
    QList<OtherInfo> m_otherInfo;

};

#endif // OTHERINFOMODEL_H
