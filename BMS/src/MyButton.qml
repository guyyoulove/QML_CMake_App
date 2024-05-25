import QtQuick.Controls 2.5
import QtQuick 2.12
Button{
    id:btn
    property alias e_btnBackW : back.width
    property alias e_btnBackH : back.height
    property string e_btnBackColor : "white"
    property color e_btnBorderColor :"white"
    property alias e_text: backText.text
    property alias e_textColor: backText.color
    width: e_btnBackW
    height: e_btnBackH
    background: Rectangle{
        id:back
        radius: 5
        color:{
            if(!enabled)
                return "#C2C2C2"
            else
                return btn.pressed?"#C2C2C2":e_btnBackColor
        }

        border.color:e_btnBorderColor
        Text{
            id:backText
            font.family: "微软雅黑"
            font.pixelSize: 16
            anchors.centerIn: parent
            color:"white"
        }
    }
}
