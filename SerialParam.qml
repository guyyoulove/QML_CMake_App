import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
ComboBox{ //串口参数下拉框
    id:control
    property alias e_name: name.text
    property alias e_content:control.model  //下拉列表中显示的内容
    property alias e_index: control.currentIndex  //目前选中的index
    Text{
        id:name
        anchors.left: parent.left
        anchors.bottom: parent.top
        anchors.bottomMargin: 5
        font.family: "微软雅黑"
        font.pixelSize: 12
        color:"#666767"
    }
    contentItem: Text { //combobox中显示的内容样式
        leftPadding:15
        text: control.displayText
        font.family: "微软雅黑"
        font.pixelSize: 12
        color: "black"
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }
    background: Image {
        id: back
        source:control.popup.visible?"/icon/image/pulldowny.jpg": "/icon/image/pulldown.jpg"
    }
    indicator: Image{  //下拉按键
        anchors.verticalCenter: back.verticalCenter
        anchors.left: back.left
        anchors.leftMargin: 109
        source: "/icon/image/iconmore.png"
    }
    delegate: ItemDelegate {  // popup 中每个item的样式
        width:130
        height:20
        contentItem: Text{
            anchors.verticalCenter: rec.verticalCenter
            verticalAlignment:Text.AlignVCenter
            text: modelData
            font.family: "微软雅黑"
            font.pixelSize: 12
            color:control.currentIndex==index?"#00B5AC":"black"
        }

        background: Rectangle{
            id:rec
            color:hovered?"#D6EAFE":"white"
        }


    }
}
