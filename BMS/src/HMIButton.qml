import QtQuick.Controls 2.5
import QtQuick 2.12

Button{ //界面左边竖条的按钮，即选择界面的按钮
    property int buttonFlag: 0
    property alias e_img: icon.source
    property alias e_down: control.down
    property alias e_buttonFlag: control.buttonFlag
    property alias e_name: name.text
    id:control
    width:160
    height:40

    onClicked:{
        if(down)
            down=false;
        else{
            recordButton(control.buttonFlag);
            down=true;
        }
    }
    background: Rectangle{
        color:  control.down ?"#F5F8FD":"white"
        Image {
            id:icon
            x:14
            y:11
        }
        Text{
            id:name
            anchors.left: icon.right
            anchors.leftMargin: 15
            anchors.verticalCenter: icon.verticalCenter
            font.family: "微软雅黑"
            font.pixelSize: 14
            //text:qsTr("电池信息")
            color:control.down ?"black":"#666767"
        }
    }
}
