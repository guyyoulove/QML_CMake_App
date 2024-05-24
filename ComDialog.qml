import QtQuick 2.7

import QtQuick.Controls 2.1

import QtQuick.Controls.Styles 1.4
Item{  //界面左竖条

    property int e_power: 0
    property alias e_serialBtn: serialBtn.down

    Image {
        x:14
        y:3
        z:1
        source: "/icon/image/object.png"
    }

    Column{
        id:col
        x:0
        y:43
        spacing: 0
        width:160
        height:500
        HMIButton{
            e_buttonFlag: 0
            id:batteryInfoBtn
            e_img:batteryInfoBtn.down?"/icon/image/iconcelly.png":"/icon/image/iconcelln.png"
            e_down: true
            e_name:qsTr("电池信息")
            onClicked: {
                stack.currentIndex=0;

                if(isSerialOpen){
                    //如果打开了串口，则应该读电池信息
                    if(isReadTempInfo){
                        isReadTempInfo=false;
                        isReadBatteryInfo=true;
                        bms.stopReadInfo();
                    }
                    else{
                        isReadBatteryInfo=true;
                        bms.readBatteryInfo(!isHoldNo2_18);
                    }


                }
            }

        }
        HMIButton{
            e_buttonFlag: 1
            id:paramConfigBtn
            e_img:paramConfigBtn.down?"/icon/image/iconparay.png":"/icon/image/iconparan.png"
            e_name:qsTr("参数配置")
            onClicked: {
                stack.currentIndex=1;

                if(isReadBatteryInfo ||isReadTempInfo){
                    //如果之前正在读电池或温度信息，则应该停止
                    isReadBatteryInfo=false;
                    isReadTempInfo=false;
                    bms.stopReadInfo();
                }

            }
        }
        HMIButton{
            e_buttonFlag: 2
            id:correctBtn
            e_img:correctBtn.down?"/icon/image/iconcorrecty.png":"/icon/image/iconcorrectn.png"
            e_name:qsTr("校准")
            onClicked: {
                stack.currentIndex=2;

                if(isSerialOpen){
                    //如果串口打开了，则读温度信息和电流信息
                    if(isReadBatteryInfo){
                        //如果之前正在读电池信息，则应该停止读电池信息
                        isReadBatteryInfo=false;
                        isReadTempInfo=true;
                        bms.stopReadInfo();

                    }
                    else{  //如果没有读电池信息，则读温度信息和电流信息
                        isReadTempInfo=true;
                        bms.correct_readTemp();
                    }


                }
            }

        }
        HMIButton{
            e_buttonFlag: 3
            id:logBtn
            e_img:logBtn.down?"/icon/image/iconlogy.png":"/icon/image/iconlogn.png"
            e_name:qsTr("日志")
            onClicked: {
                stack.currentIndex=3;
                if(isReadBatteryInfo || isReadTempInfo){
                    //如果之前正在读电池信息，则应该停止读电池信息
                    isReadBatteryInfo=false;
                    isReadTempInfo=false;
                    bms.stopReadInfo();
                }
            }
        }
        HMIButton{
            e_buttonFlag: 4
            id:afeRomBtn
            e_img:afeRomBtn.down?"/icon/image/icony.png":"/icon/image/iconn.png"
            e_name:qsTr("中颖AfeRom")
            onClicked: {
                stack.currentIndex=4;
                if(isReadBatteryInfo || isReadTempInfo){
                    //如果之前正在读电池信息，则应该停止读电池信息
                    isReadBatteryInfo=false;
                    isReadTempInfo=false;
                    bms.stopReadInfo();
                }
            }
        }


    }
    Text{
        x:15
        y:281
        text:qsTr("串口操作")
        font.family: "微软雅黑"
        font.pixelSize: 14
    }
    Image{
        id:serialIcon
        x:14
        y:370-55
        source:{
            if(!isSerialOpen){ //串口未打开
                return "/icon/image/circlegray.jpg"
            }
            else
                return "/icon/image/circlegreen.jpg"
        }
    }
    Text{
        x:26.3
        //y:367-55
        anchors.verticalCenter: serialIcon.verticalCenter
        text:{
            if(!isSerialOpen){ //串口未打开
                return qsTr("-- --")
            }
            else
                return qsTr("已打开")
        }
        color: serialBtn.down?"#29A999":"#D7D7D7"
        font.family: "微软雅黑"
        font.pixelSize: 14
    }

    Button{ //串口按钮
        x:118
        y:364-55
        id:serialBtn
        down:isSerialOpen
        onClicked: {

            if(!isValid){ //还未授权
                msg.text="还未授权，无法使用！请点击选项->授权";
                msg.open();
                isSerialOpen=false;
            }
            else{  //已授权
                if(isSerialOpen){
                    //down=false;
                    bms.closeSerialPort();
                }
                else{
                    //down=true;
                    bms.transaction(comPort.currentText,baudRate.currentText,dataBit.currentText,stopBit.currentText,
                                    parityBit.currentText,stack.currentIndex);
                }
            }
        }
        background: Image {
            x:0
            y:0
            source:serialBtn.down? "/icon/image/open.png":"/icon/image/disable.png"
        }

    }


    Column{
        x:14
        y:370
        spacing: 38
        SerialParam{id:comPort;e_name:qsTr("串口号");e_content: serialPortInfo;}
        SerialParam{id:baudRate;e_name:qsTr("波特率");e_content:baudInfo;}//[ "1200", "2400", "4800","9600","14400","19200","38400","57600","115200"]
        SerialParam{id:parityBit;e_name:qsTr("校验位") ;e_content: [ "None", "Odd", "Even" ];}
        SerialParam{id:dataBit;e_name:qsTr("数据位长度");e_content: [ "8"];}
        SerialParam{id:stopBit;e_name:qsTr("停止位长度");e_content: [ "1"];}

    }
    Text{
        x:14
        y:670
        text:qsTr("当前电池")
        font.family: "微软雅黑"
        font.pixelSize: 14
    }


    Text{
        x:14
        y:698
        text:{
            if(!isSerialOpen){
                //串口没开
                return  qsTr("电量: -- ")
            }
            else{
                //串口打开
                return  qsTr("电量:")+e_power
            }
        }

        font.family: "微软雅黑"
        font.pixelSize: 14
    }
    PowerProgress{
        x:14
        y:722
        e_value:e_power
    }



//    Button{
//        id:timeBtn
//        font.family: "微软雅黑"
//        font.pixelSize: 15
//        text: "查看接收数据时间"
//        x:14
//        y:770
//        onClicked: {
//            rxTime.show();
//        }
//    }
    Button{
        x:25
        y:791
        background: Image{
            source:  "/icon/image/iconpack.png"
        }
    }


    function recordButton(flag){
        for(var i=0;i<5;i++){
            if(i!==flag)
                col.children[i].e_down=false;
        }
    }
    Component.onCompleted: {
        serialPortInfo.addAll();
        baudInfo.addAll();
        baudRate.e_index=baudInfo.getIndexOf115();
        comPort.e_index=0;
        timer.start()

    }

    Timer{
        id:timer
        interval: 1000
        repeat: true
        onTriggered: {
            // console.log("检查串口")
            serialPortInfo.update();
        }
    }
}

