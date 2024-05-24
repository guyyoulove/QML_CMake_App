import QtQuick.Window 2.12
import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.5

Window{
    property alias e_model:timeInfoModel
    id:timeInfoWin
    x:bmsApp.x+756
    y:bmsApp.y
    width: 441
    height: 572
    flags: Qt.FramelessWindowHint

        Text{
            x:43
            y:59
            text:qsTr("时间信息")
            color:"#040404"
            font.family: "微软雅黑"
            font.pixelSize: 16
        }
        Button{
            x:385
            y:40
            width: 11
            height:11
            onClicked:{
                timeInfoWin.close();
            }

            background: Image{
                source: "icon/image2/iconclose.png"
            }
        }
        ListView{
            id:timeInfoTable
            x:44
            y:92
            width:352
            height:440
            delegate:Text{
                height: 40
                text:value
                font.family: "微软雅黑"
                font.pixelSize: 16
                color:"#666666"
                horizontalAlignment:Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            model: timeInfoModel
        }







    ListModel{
        id:timeInfoModel
    }








}


