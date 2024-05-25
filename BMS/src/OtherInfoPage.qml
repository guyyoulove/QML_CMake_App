import QtQuick.Window 2.12
import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.5

Window{
    property alias e_model: otherInfoModel
    id:otherInfoWin
    x:bmsApp.x+756
    y:bmsApp.y+315
    width: 441
    height: 572
    flags: Qt.FramelessWindowHint
    Image{
        x:0
        y:0
        source: "icon/image2/otherinfoback.jpg"
        Text{
            x:43
            y:59
            text:qsTr("电池其他信息")
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
                otherInfoWin.close();
            }

            background: Image{
                source: "icon/image2/iconclose.png"
            }
        }
        TableView{
            id:otherInfoTable
            x:44
            y:92
            width:352
            height:440
            backgroundVisible: false
            frameVisible: false
            horizontalScrollBarPolicy:Qt.ScrollBarAlwaysOff
            headerDelegate:Text{
                height: 40
                text:styleData.value
                font.family: "微软雅黑"
                font.pixelSize: 16
                color:"#666666"
                horizontalAlignment:Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            itemDelegate:Loader{
                property var getValue: 0;
                property var colNo:styleData.column
                sourceComponent: {
                    if(styleData.column===0)
                    {
                        getValue=styleData.value;
                        return dispText
                    }
                    else if(styleData.column<3)
                    {
                        if(!isSerialOpen){
                            //串口未开
                            getValue="-- --"
                        }
                        else{//串口打开
                            getValue=styleData.value;
                        }
                        return dispText
                    }
                    else if(styleData.column===3)
                    {
                        return dispRecord
                    }
                    else if(styleData.column===4)
                    {
                        return dispScan
                    }

                }
            }



            rowDelegate: Rectangle{
                width:352
                height:40
                color:"white"
            }

            TableViewColumn {
                role: "nameRole"
                title:qsTr("名称")
                width: 71
            }
            TableViewColumn {
                role: "valueRole"
                title: qsTr("数值")
                width: 71
            }

            TableViewColumn {
                role: "unitRole"
                title:qsTr("单位")
                width: 70
            }
            TableViewColumn {
                title: qsTr("记录")
                width: 70
            }
            TableViewColumn {
                title: qsTr("扫描")
                width: 70
            }
            model: otherInfoModel

        }
        Canvas{
            x:0
            y:0
            width:441
            height:572
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<10;i++)
                    ctx.fillRect(44,172+i*40, 352, 1);
            }
        }

    }

    ListModel{
        id:otherInfoModel
    }




    property var nameInOtherInfoModel: ["总电压","总电流","最高电压","最低电压","平均电压","电芯压差","循环次数","满充容量","剩余容量","RSOC"]
    property var unitInOtherInfoModel: ["V","A","mV","mV","mV","mV","--","mAh","mAh","%"]

    Component.onCompleted: {
        for(var i=0;i<10;i++){
            otherInfoModel.append({nameRole:nameInOtherInfoModel[i],unitRole:unitInOtherInfoModel[i],valueRole:""});}
    }

//    Connections{
//        target: bms
//        onNo89_91:{
//            otherInfoModel.setProperty(6, "valueRole", v1)
//            otherInfoModel.setProperty(7, "valueRole", v2)
//            otherInfoModel.setProperty(8, "valueRole", v3)
//        }
//    }

}


