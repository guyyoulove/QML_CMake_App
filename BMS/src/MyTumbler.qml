import QtQuick.Controls 2.5
import QtQuick 2.12
//Tumbler {
//    id:tumbler
//    property alias e_text: title.text
//    property alias e_model:tumbler.model
//    Text{
//        id:title
//        anchors.bottom: parent.top
//        anchors.bottomMargin: 20
//        anchors.horizontalCenter: parent.horizontalCenter
//        font.family: "微软雅黑"
//        font.pixelSize: 15
//        color: "#00B5AC"
//    }
//    delegate: Rectangle{
//        border.color: (Tumbler.displacement==0)?"#73B3F6":"white"
//        border.width: 2
//        radius: 5
//        TextInput{
//            anchors.centerIn: parent
//            font.family: "微软雅黑"
//            font.pixelSize: 16
//            text: modelData
//            horizontalAlignment: Text.AlignHCenter
//            verticalAlignment: Text.AlignVCenter
//        }
//    }
//}

Rectangle{
    property alias e_text: title.text
    property alias e_input: input.text
    Text{
        id:title
        anchors.bottom: parent.top
        anchors.bottomMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "微软雅黑"
        font.pixelSize: 15
        color: "#00B5AC"
    }
    color:"#F2F7FE"
    border.color: "#C3C8D0"
    border.width: 1
    radius: 5
    width: 70
    height: 30
    TextInput{
        id:input
        anchors.fill: parent
        font.family: "微软雅黑"
        font.pixelSize: 16
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
