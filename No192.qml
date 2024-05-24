import QtQuick 2.7
import QtQuick.Controls 2.1

Image{
    property alias e_img: img.source
    property alias e_name: name.text
    property bool e_isOpen:false
    property bool e_down: false
    property alias e_btnDown: btn.down

    source: "/icon/image/tubeback.jpg"

    Image{
        id:img
        x:27
        y:23
    }
    Text{
        id:name
        x:27
        y:95
        font.family: "微软雅黑"
        font.pixelSize: 18
        color:"#666666"
    }
    //    Button{
    //        id:btn
    //        x:27
    //        y:138
    //        onClicked: {
    //            if(down)
    //                down=false;
    //            else
    //                down=true;
    //        }
    //        background: Image {
    //            x:0
    //            y:0
    //            source:{
    //                if(!isSerialOpen){//没开串口
    //                    return "/icon/image/disable.png";
    //                }
    //                else{
    //                    //开了串口
    //                    return btn.down? "/icon/image/open.png":"/icon/image/disable.png";
    //                }
    //            }
    //        }

    //    }
    Button{
        id:btn
        x:27
        y:138

        onClicked: {
            if(down){
                down=false;
                e_down=false;
            }
            else{
                down=true;
                e_down=true;
            }
        }

    background:Image {
        source:{
            if(!isSerialOpen){//没开串口
                return "/icon/image/disable.png";
            }
            else{
                //开了串口
                return btn.down? "/icon/image/open.png":"/icon/image/disable.png";
            }
        }
    }

}
    Text{
        x:105
        anchors.verticalCenter:btn.verticalCenter
        text:{
            if(!isSerialOpen){//没开串口
                return "--- ---";
            }
            else{
                //开了串口
                return btn.down?qsTr("已关断"):qsTr("已解除");
            }
        }



        color:{
            if(!isSerialOpen){//没开串口
                return "#D7D7D7";
            }
            else{
                //开了串口
                return e_isOpen?"#1AB2A1":"#F84712";
            }
        }

        font.family: "微软雅黑"
        font.pixelSize: 14

    }

}
