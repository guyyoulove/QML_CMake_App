import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.1

Button{
    id:btn
    background: Rectangle{
        width:96
        height:36
        color:btn.pressed?"#C2C2C2": "#1AB2A1"
        radius: 5
        Text{
            anchors.centerIn: parent
            text:qsTr("校准")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"white"
        }
    }
}
