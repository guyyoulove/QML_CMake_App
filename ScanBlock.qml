import QtQuick 2.7
import QtQuick.Controls 2.1

Text{  //扫描输入元和按钮
    property alias e_unit: unit.text
    property alias e_unitLocation: unit.x
    property alias e_input: setValue.text
    font.family: "微软雅黑"
    font.pixelSize: 18
    color:"#999999"
    Image{
        anchors.left: parent.right
        anchors.leftMargin: 26
        anchors.verticalCenter: parent.verticalCenter
        source: "/icon/image/input.jpg"
        TextInput{  //注意输入j的话，界面上的全部字符都会消失
            id:setValue
            anchors.fill: parent
            font.family: "微软雅黑"
            font.pixelSize: 14
            verticalAlignment:TextInput.AlignVCenter
            leftPadding: 5
            text:qsTr("单行输入")
            color:"#B8B8B8"
            onFocusChanged: {
                if(focus){
                    if(text=="单行输入"){
                        text=""
                    }
                    color="black"
                }
                else{
                    if(text==""){
                        text="单行输入"
                        color="#B8B8B8"
                    }
                }
            }
        }
        Text{
            id:unit
            //x: 116
            font.family: "微软雅黑"
            font.pixelSize: 14
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment:TextInput.AlignVCenter
            color:"#666666"
        }
    }



}
