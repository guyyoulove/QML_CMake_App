import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.5
import QtQuick 2.7

Text{ //参数配置下拉框
    property alias e_text: name.text
    property alias e_boxX: control.x
    property alias e_content:control.model  //下拉列表中显示的内容
    property alias e_index: control.currentIndex  //目前选中的index
    property alias e_boxWidth: control.implicitWidth
    property alias e_boxBackWidth: back.width
    property alias e_boxHeight: control.implicitHeight
    property int e_row:0
    property int e_col:0
    id:name
    font.family: "微软雅黑"
    font.pixelSize: 16
    color:"#333333"
    verticalAlignment: Text.AlignVCenter
    ComboBox{
        id:control
        x:72
        implicitWidth: 140
        implicitHeight :30
        anchors.verticalCenter: parent.verticalCenter

        contentItem: Text { //combobox中显示的内容样式
            leftPadding:10
            text: control.displayText
            font.family: "微软雅黑"
            font.pixelSize: 14
            color: "black"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            id: back
            width:140
            height: 30
            color:"#F2F7FE"
            border.color: "#C3C8D0"
            border.width: 1
            radius: 5
        }
        indicator: Image{  //下拉按键
            anchors.verticalCenter: back.verticalCenter
            anchors.right: back.right
            anchors.rightMargin: 5
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
                font.pixelSize: 14
                color:control.currentIndex==index?"#00B5AC":"black"
            }

            background: Rectangle{
                id:rec
                color:hovered?"#D6EAFE":"white"
            }


        }
        onCurrentIndexChanged: {
            bms.saveParam(control.currentIndex,e_row,e_col,viewFlag);
        }
    }
}











