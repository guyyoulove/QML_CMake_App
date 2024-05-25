import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.12
Image{
    id:back
    property alias e_value: powerProgress.value
    source: {
        if(!isSerialOpen){
            //串口未打开
            return "/icon/image/noconnect.png"
        }
        else{
            //串口打开
            if(e_power<30)
                return "/icon/image/lowpower.png"
            else
                return "/icon/image/power.png"
        }
    }

    ProgressBar{
        id:powerProgress
        anchors.left: parent.left
        anchors.leftMargin: 3
        anchors.verticalCenter: parent.verticalCenter
        maximumValue :100
        minimumValue :0
        visible: {
            if(!isSerialOpen){
                //串口未打开
                return false;
            }
            else
                return true;
        }

        style: ProgressBarStyle {
            background: Rectangle {
                radius: 3
                color: "white"
                implicitWidth: 123
                implicitHeight: 17
            }
            progress: Rectangle {
                radius: 3
                color: {
                    if(powerProgress.value<30)
                        return "#F84712"
                    else
                        return "#1AB2A1"
                }
            }
        }
    }
}
