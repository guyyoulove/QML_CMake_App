#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "serialportinfo.h"
#include "baudinfo.h"
#include "comthread.h"
#include "batteryinfo.h"
#include "bms.h"
#include "tempinfo.h"
#include "loginfomodel.h"
#include "otherinfomodel.h"
#include "login.h"
#include <QDebug>
int main(int argc, char* argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	qmlRegisterType<Login>("an.qt.Login", 1, 0, "Login");
	qmlRegisterType<SerialPortInfo>("an.qt.SerialPortInfo", 1, 0, "SerialPortInfo");
	qmlRegisterType<BaudInfo>("an.qt.BaudInfo", 1, 0, "BaudInfo");
	qmlRegisterType<BMS>("an.qt.BMS", 1, 0, "BMS");
	qmlRegisterType<LogInfoModel>("an.qt.LogInfoModel", 1, 0, "LogInfoModel");
	qmlRegisterType<BatteryInfoModel>("an.qt.BatteryInfoModel", 1, 0, "BatteryInfoModel");
	qmlRegisterType<TempInfoModel>("an.qt.TempInfoModel", 1, 0, "TempInfoModel");
	qmlRegisterType<OtherInfoModel>("an.qt.OtherInfoModel", 1, 0, "OtherInfoModel");

	engine.load(QUrl(QLatin1String("qrc:/main.qml")));



	return app.exec();
}
