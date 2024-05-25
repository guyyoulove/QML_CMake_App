#ifndef LOGIN_H
#define LOGIN_H

#include <QObject>
#include <QNetworkInterface>
#include <QFile>
#include <QCoreApplication>
#include "md5.h"
class Login : public QObject
{
    Q_OBJECT
public:
    explicit Login(QObject *parent = nullptr);
    Q_INVOKABLE QString createID();
    Q_INVOKABLE void check(QString id,QString license);
    Q_INVOKABLE bool isValid();
signals:
    void msg(QString s);
public slots:
};

#endif // LOGIN_H
