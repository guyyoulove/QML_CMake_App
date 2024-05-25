#include "login.h"

Login::Login(QObject *parent) : QObject(parent)
{

}

QString Login::createID()
{
    foreach(QNetworkInterface networkInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if (!(networkInterface.flags() & QNetworkInterface::IsLoopBack))
            return networkInterface.hardwareAddress();
    }
    return "";
}

void Login::check(QString id,QString license)
{
    //qDebug()<<"check"<<id<<" "<<license;
    QFile licenseFile(QCoreApplication::applicationDirPath()+"/licenseFile.txt");
    /*if(licenseFile.exists()){
        //已经有文件
        if (!licenseFile.open(QIODevice::ReadOnly)){
            //emit exportRecordStatus("请关闭临时数据文件("+QCoreApplication::applicationDirPath()+"/batteryfile.dat)！再执行导出操作");
            emit msg("请关闭配置文件。");
            return;
        }
        //读文件中的ID和License
        QTextStream in(&licenseFile);
        QString saveId=in.readLine();
        QString saveLicense=in.readLine();
        qDebug()<<"save value "<<saveId<<" "<<saveLicense;

        //读电脑中的ID和License
        QString currentId;
        foreach(QNetworkInterface networkInterface, QNetworkInterface::allInterfaces())
        {
            // Return only the first non-loopback MAC Address
            if (!(networkInterface.flags() & QNetworkInterface::IsLoopBack)){
                currentId=networkInterface.hardwareAddress();
                break;
            }
        }
        QString currentLicense=QString::fromStdString(MD5(QString(id+"l2m0j2").toStdString()).toStr());

        qDebug()<<"current value "<<currentId<<" "<<currentLicense;

        if((saveId==currentId) && (saveLicense==currentLicense)){
            //验证成功
            emit msg("验证成功");
        }
        else{
            emit msg("验证失败");
        }

    }*/
    //else{//没有文件，创建文件

        //计算license
        id.append("l2m0j2");
        QString calLicense=QString::fromStdString(MD5(id.toStdString()).toStr());
        if(license==calLicense){
            if (!licenseFile.open(QIODevice::WriteOnly | QIODevice::Text)){
               // emit msg("写配置文件失败");
                return;
            }


            QTextStream out(&licenseFile);
            out << id.left(id.size()-6) << "\n" <<license;
            emit msg("验证成功");
        }
        else{
             emit msg("验证失败");
        }



   // }


}

bool Login::isValid()
{
    return true;
    /*QFile licenseFile(QCoreApplication::applicationDirPath()+"/licenseFile.txt");
    if(licenseFile.exists()){
        //已经有文件
        if (!licenseFile.open(QIODevice::ReadOnly)){
            emit msg("正在验证授权码，请关闭配置文件。");
            return false;
        }
        //读文件中的ID和License
        QTextStream in(&licenseFile);
        QString saveId=in.readLine();
        QString saveLicense=in.readLine();

        //读电脑中的ID和License
        QString currentId;
        foreach(QNetworkInterface networkInterface, QNetworkInterface::allInterfaces())
        {
            // Return only the first non-loopback MAC Address
            if (!(networkInterface.flags() & QNetworkInterface::IsLoopBack)){
                currentId=networkInterface.hardwareAddress();
                break;
            }
        }
        QString currentLicense=QString::fromStdString(MD5(QString(currentId+"l2m0j2").toStdString()).toStr());

       // qDebug()<<"current value "<<currentId<<" "<<currentLicense;

        if((saveId==currentId) && (saveLicense==currentLicense)){
            //验证成功
            return true;
        }
        else{
            return false;
        }

    }
    else{//没有文件，创建文件
       // qDebug()<<"没有该文件";

        return false;
     }*/

}
