import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick 2.12
Window {
    // property alias e_yearModel:yearModel
    property int e_flag: 0 //0代表选起始时间  1代表选截止时间

    id:selectTimeWin
    x:bmsApp.x+(bmsApp.width-width)/2
    y:bmsApp.y+(bmsApp.height-height)/2
    width: 500
    height: 200
    flags: Qt.FramelessWindowHint

    Rectangle {
        width: 500
        height: 200
        anchors.centerIn: parent
        border.color:"black"
        border.width: 1
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
                    selectTimeWin.x=selectTimeWin.x+mouse.x-firstX;
                    selectTimeWin.y=selectTimeWin.y+mouse.y-firstY;
                }
            }
            onReleased: {
                first=true;
                firstX=0;
                firstY=0;
            }
        }

        Row {
            id: row
            spacing: 10
            anchors.centerIn: parent
            MyTumbler{
                id:year
                e_text: qsTr("年")
                //e_model: yearModel
            }
            MyTumbler{
                id:mon
                e_text: qsTr("月")
                //e_model: monModel
            }
            MyTumbler{
                id:day
                e_text: qsTr("日")
                //e_model: dayModel
            }
            MyTumbler{
                id:hour
                e_text: qsTr("时")
                //e_model: hourModel
            }
            MyTumbler{
                id:minute
                e_text: qsTr("分")
                //e_model: minuteModel
            }
            MyTumbler{
                id:sec
                e_text: qsTr("秒")
                //e_model: secModel
            }

        }


        MyButton{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: row.bottom
            anchors.topMargin: 30
            e_btnBackColor: "#00B5AC"
            e_text: qsTr("确定")
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
//                if(0==e_flag){  //选起始时间
//                    log.e_startTime=yearModel.get(year.currentIndex).modelData+"-"+monModel.get(mon.currentIndex).modelData
//                            +"-"+dayModel.get(day.currentIndex).modelData+" "+hourModel.get(hour.currentIndex).modelData
//                            +":"+minuteModel.get(minute.currentIndex).modelData+":"+secModel.get(sec.currentIndex).modelData
//                }
//                else if(1==e_flag){  //选截止时间
//                    log.e_stopTime=yearModel.get(year.currentIndex).modelData+"-"+monModel.get(mon.currentIndex).modelData
//                            +"-"+dayModel.get(day.currentIndex).modelData+" "+hourModel.get(hour.currentIndex).modelData
//                            +":"+minuteModel.get(minute.currentIndex).modelData+":"+secModel.get(sec.currentIndex).modelData

//                }
                if(0==e_flag){  //选起始时间
                    log.e_startTime=year.e_input+"-"+mon.e_input
                            +"-"+day.e_input+" "+hour.e_input
                            +":"+minute.e_input+":"+sec.e_input
                }
                else if(1==e_flag){  //选截止时间
                    log.e_stopTime=year.e_input+"-"+mon.e_input
                            +"-"+day.e_input+" "+hour.e_input
                            +":"+minute.e_input+":"+sec.e_input
                }
                selectTimeWin.close();
            }
        }

        Button{
            anchors.right: parent.right
            anchors.rightMargin: 10
            y:15
            width: 11
            height:11
            onClicked:{
                selectTimeWin.close();
            }

            background: Image{
                source: "icon/image2/iconclose.png"
            }
        }

        //        ListModel{
        //            id:yearModel
        //        }
        //        ListModel{
        //            id:monModel
        //        }
        //        ListModel{
        //            id:dayModel
        //        }
        //        ListModel{
        //            id:hourModel
        //        }
        //        ListModel{
        //            id:minuteModel
        //        }
        //        ListModel{
        //            id:secModel
        //        }


        //        Component.onCompleted: {
        //            for(var i=1;i<13;i++)
        //                monModel.append({"modelData":i});
        //            for(i=1;i<32;i++)
        //                dayModel.append({"modelData":i});
        //            for(i=0;i<24;i++)
        //                hourModel.append({"modelData":i});
        //            for(i=0;i<60;i++)
        //                minuteModel.append({"modelData":i});
        //            for(i=0;i<60;i++)
        //                secModel.append({"modelData":i});
        //        }
    }
}
