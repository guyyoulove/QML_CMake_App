import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick 2.12
Window {
    property int e_showFlag: 0
    property int e_pageNo: 0
    property bool e_deleteAll: false

    id:tipsWin
    x:bmsApp.x+(bmsApp.width-width)/2
    y:bmsApp.y+(bmsApp.height-height)/2
    width: 445
    height: 196
    flags: Qt.FramelessWindowHint

    Image {
        x:0
        y:0
        source: "icon/image2/tipsback.jpg"
        anchors.centerIn: parent
        MouseArea{
            z:-1
            property bool first: true
            property real firstX : 0
            property real firstY : 0

            anchors.fill: parent

            onPositionChanged: {
                if(first){
                    first=false;
                    firstX=mouse.x
                    firstY=mouse.y
                }
                else{
                    tipsWin.x=tipsWin.x+mouse.x-firstX;
                    tipsWin.y=tipsWin.y+mouse.y-firstY;
                }
            }
            onReleased: {
                first=true;
                firstX=0;
                firstY=0;
            }
        }

        Text{
            x:30
            y:35
            text: qsTr("删除ROM日志")
            font.family: "微软雅黑"
            font.pixelSize: 18
        }
        Text {
            x:30
            y:78
            text: qsTr("删除后，将清空你所选中的日志，请确认是否继续删\r\n除操作？")
            font.family: "微软雅黑"
            font.pixelSize: 16
        }

        MyButton{
            x:199
            y:135
            e_btnBackColor: "#FFFFFF"
            e_btnBorderColor: "#999999"
            e_textColor: "black"
            e_text: qsTr("取 消")
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
                 tipsWin.close();
            }

        }
        MyButton{
            x:313
            y:135
            e_btnBackColor: "#F84712"
            e_text: qsTr("删除日志")
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
                if(e_deleteAll){
                    //删除所有
                    logInfoModel.deleteAll(e_pageNo,e_showFlag);
                    log.e_deleteAllLogBtn_Down=false;
                }
                else{
                    //删除选中
                    logInfoModel.deleteSelect(e_pageNo,e_showFlag);
                }

                  tipsWin.close();
            }

        }
    }
}
