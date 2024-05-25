import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.1
Item { //校准界面
    id:root
    //property int tempCorrectFlag: 0 //按下温度校准中某行记录按钮的标志,0则表示没按下，正常值为index+1，一次只能按下一个，因为一次只能校准一个
    property alias e_tCorrectModel: tCorrectModel
    property int aCorrectFlag: 0
    //property alias e_aCorrectFlag: aCorrectFlag
    property alias e_aCorrectModel: aCorrectModel
    property bool tempCorrect: false  //是否在进行温度校准 是为true  不是为false
    property bool aCorrect: false  //是否在进行电流校准 是为true  不是为false
    property bool timeCorrect: false  //是否在进行时间校准 是为true  不是为false

    property alias e_rtcTime: rtcTime.e_input

    Image{//温度校准
        x:0
        y:0
        source: "icon/image2/tempcorrectback.jpg"
        MyButton{
            id:tCorrectButton
            x:197
            y:414
            e_btnBackColor: "#1AB2A1"
            e_text: qsTr("校准")
            e_btnBackW: 96
            e_btnBackH: 36
            onClicked: {
                for(var i=0;i<8;i++){//判断是否有勾选校准项
                    if(tCorrectModel.get(i).value3)
                        break;
                }
                if(8==i){
                    msg.text=qsTr("无已勾选校准项或校准数值输入不正确，无法校准，请修改！")
                    msg.open();
                    return;
                }


                for(i=0;i<8;i++){ //判断输入是否有误
                    if(tCorrectModel.get(i).value3){ //需要校准
                        if(tCorrectModel.get(i).value2String!=="输入" && tCorrectModel.get(i).value2String!==""){ //校准值正确
                            console.log("校准值",tCorrectModel.get(i).value2)
                        }
                        else{
                            msg.text=qsTr("无已勾选校准项或校准数值输入不正确，无法校准，请修改！")
                            msg.open();
                            return;
                        }
                    }
                }
                console.log("校准返回");
                if(isSerialOpen){  //串口已打开
                    //停止读实时温度
                    isReadBatteryInfo=false;
                    isReadTempInfo=false;
                    tempCorrect=true;
                    bms.stopReadInfo();
                    tCorrectButton.enabled=false;
                    tCorrectTimer.start();

                }
                else{
                    msg.text=qsTr("未打开串口！");
                    msg.open();
                }
            }
        }



        Text{
            x:26
            y:33
            text:qsTr("温度校准")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }
        ListView{
            id:tCorrectList
            x:5
            y:66
            width:481
            height:328
            model:tCorrectModel

            delegate: Control{
                x:0
                y:0
                width:481
                height: 41
                onHoveredChanged: {
                    if(hovered)
                        tCorrectRec.color="#D6EAFE"
                    else
                        tCorrectRec.color="white"
                }

                Rectangle{
                    anchors.fill: parent
                    id:tCorrectRec

                    ParamConfigInput{
                        x:29
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_recX: 90
                        e_text: qsTr("温度探头")+(index+1)
                        e_unit: qsTr("℃")
                        e_input: ""+value //返回值
                    }
                    ParamConfigInput{
                        x:259
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_recX: 0
                        e_text: qsTr("")
                        e_unit:  qsTr("℃")
                        e_input:qsTr("输入") //校准值
                        onE_inputChanged: {
                            value2String=e_input;
                            if(e_input!="输入" && e_input!="")
                                value2=parseInt(e_input);
                            else{
                                value2=0;
                            }
                        }

                    }
                    Button{
                        id:btn
                        anchors.verticalCenter: parent.verticalCenter
                        x:397
                        down:value3
                        //                        onReleased:{
                        //                            if(down){
                        //                                tempCorrectFlag=0;
                        //                            }
                        //                            else{
                        //                                tempCorrectFlag=(index+1);
                        //                            }
                        //                        }
                        //                        down: {
                        //                            if(tempCorrectFlag==(index+1)){
                        //                                return true;
                        //                            }
                        //                            else
                        //                                return false;
                        //                        }

                        onClicked:{
                            if(value3){
                                //down=false;
                                value3=false;
                                //bms.set_correctTempIndex(index,false);
                            }
                            else{
                                //down=true;
                                value3=true;
                                //bms.set_correctTempIndex(index,true);
                            }
                        }

                        background: Image{
                            source: btn.down?"icon/image2/select.png":"icon/image2/noselect.png"
                        }
                    }
                    Text{
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: btn.right
                        anchors.leftMargin: 10
                        text:qsTr("校准成功")
                        font.family: "微软雅黑"
                        font.pixelSize: 13
                        color: "#1AB2A1"
                        visible: value4
                    }

                }
            }

        }

        Canvas{
            x:0
            y:0
            width:491
            height:474
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<8;i++)
                    ctx.fillRect(27,106+i*41, 436, 1);
            }
        }
        ListModel{
            id:tCorrectModel

        }
    }
    Image{  //电流校准
        x:508
        y:0
        source: "icon/image2/acorrectback.jpg"
        MyButton{
            id:iCorrectButton
            x:197
            y:250
            e_btnBackColor: "#1AB2A1"
            e_text: qsTr("校准")
            e_btnBackW: 96
            e_btnBackH: 36
            //            onClicked: {
            //                if(isSerialOpen){
            //                    //第一步 得到参数no141得到总电流
            //                    bms.getNo141();
            //                }
            //                else
            //                {
            //                    msg.text=qsTr("未打开串口！")
            //                    msg.open();
            //                }
            //            }
            onClicked: {
                if(!aCorrectModel.get(0).value3 && !aCorrectModel.get(1).value3){
                    msg.text=qsTr("无已勾选校准项或校准数值输入不正确，无法校准，请修改！")
                    msg.open();
                    return;
                }

                for(var i=0;i<2;i++){
                    if(aCorrectModel.get(i).value3){ //需要校准
                        if(aCorrectModel.get(i).value2String!=="输入" && aCorrectModel.get(i).value2String!==""){ //校准值正确
                            console.log("校准值",i,aCorrectModel.get(i).value2)
                        }
                        else{
                            msg.text=qsTr("无已勾选校准项或校准数值输入不正确，无法校准，请修改！")
                            msg.open();
                            return;
                        }
                    }
                }
                console.log("电流校准返回");
                if(isSerialOpen){  //串口已打开
                    //停止读实时温度
                    isReadBatteryInfo=false;
                    isReadTempInfo=false;
                    aCorrect=true;
                    bms.stopReadInfo();
                    iCorrectButton.enabled=false;
                    iCorrectTimer.start();

                }
                else{
                    msg.text=qsTr("未打开串口！");
                    msg.open();
                }
            }


        }

        Text{
            x:26
            y:33
            text:qsTr("电流校准")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }
        ListView{
            id:aCorrectList
            x:5
            y:66
            width:481
            height:164
            model:aCorrectModel

            delegate: Control{
                x:0
                y:0
                width:481
                height: 41
                onHoveredChanged: {
                    if(hovered)
                        aCorrectRec.color="#D6EAFE"
                    else
                        aCorrectRec.color="white"
                }

                Rectangle{
                    anchors.fill: parent
                    id:aCorrectRec

                    ParamConfigInput{
                        x:{
                            if(0==index)
                                return 61;
                            else if(1==index)
                                return 38;
                            else if(2==index)
                                return 42;
                            else
                                return 27;

                        }
                        e_recX:119-x;
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_text: name
                        e_input: value //返回值
                        e_unit: {
                            if(index<2)
                                return "mA";
                            else
                                return "";
                        }

                    }
                    ParamConfigInput{
                        x:259
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_recX: 0
                        e_text: qsTr("")
                        e_unit: {
                            if(index<2)
                                return "mA";
                            else
                                return "";
                        }
                        e_input:{
                            if(index==0)
                                return "0";
                            else
                                return "-1000";
                        }

                        onE_inputChanged: {
                            value2String=e_input;
                            if(e_input!="输入" && e_input!="")
                                value2=parseInt(e_input);
                            else{
                                value2=0;
                            }
                        }
                    }
                    Button{
                        opacity: {
                            if(index<2)
                                return 1;
                            else
                                return 0;
                        }

                        id:aCorrectBtn
                        anchors.verticalCenter: parent.verticalCenter
                        x:392
                        onReleased:{
                            if(value3){
                                aCorrectFlag=0;
                            }
                            else{
                                aCorrectFlag=(index+1);
                            }
                        }
                        down: {
                            if(aCorrectFlag==(index+1)){
                                value3=true;
                                return true;
                            }
                            else{
                                value3=false;
                                return false;
                            }
                        }

                        background: Image{
                            source: aCorrectBtn.down?"icon/image2/select.png":"icon/image2/noselect.png"
                        }
                    }


                }
            }

        }

        Canvas{
            x:0
            y:0
            width:491
            height:310
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<4;i++)
                    ctx.fillRect(27,106+i*41, 436, 1);
            }
        }
        ListModel{
            id:aCorrectModel
        }
    }
    Image{//时间校准
        x:508
        y:327
        source: "icon/image2/timecorrectback.jpg"
        MyButton{
            id:timeCorrectButton
            x:367
            anchors.verticalCenter: rtcTime.verticalCenter
            e_btnBackColor: "#1AB2A1"
            e_text: qsTr("校准")
            e_btnBackW: 96
            e_btnBackH: 36
            onClicked: {  //rtc校准
                if(isSerialOpen){  //串口已打开

                    //停止读实时温度和实时电流
                    isReadBatteryInfo=false;
                    isReadTempInfo=false;
                    timeCorrect=true;
                    bms.stopReadInfo();
                    timeCorrectButton.enabled=false;
                    rtcCerrectTimer.start();

                }
                else{
                    msg.text=qsTr("未打开串口！");
                    msg.open();
                }
            }
        }
        Text{
            id:rtc
            x:25
            y:23
            text:qsTr("RTC校准")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }
        ParamConfigInput{
            id:rtcTime
            x:43
            y:57
            e_text:qsTr("当前时间")
            e_recX:100
            e_recWidth: 169
            e_recHeight: 30
        }
        ParamConfigInput{
            id:currentTime
            x:43
            y:107
            e_text:qsTr("当前电脑时间")
            e_recX:100
            e_recWidth: 169
            e_recHeight: 30
            e_input:bms.correctDate();
        }

    }
    Timer{
        id:timer
        interval: 1000
        repeat: true
        onTriggered: {
            currentTime.e_input=bms.correctDate();
        }
    }

    onVisibleChanged: {
        if(visible){
            console.log("visible true timer.start()");
            timer.start()
        }
        else{
            console.log("visible false timer.stop()");
            timer.stop()
        }

    }

    property var nameInACorrect: ["0电流","工作电流","电流增益","电流偏移量"]
    Component.onCompleted: {
        for(var i=0;i<8;i++)
            tCorrectModel.append({value:0,value2:0,value2String:"",value3:false,value4:false})
        for(i=0;i<4;i++)
            aCorrectModel.append({name:nameInACorrect[i],value:0,value2:0,value2String:"",value3:false})
    }
    Connections{
        target: bms
        onComSleep:{
            console.log("onComSleep")
            if(tempCorrect){
                tempCorrect=false;
                for(var j=0;j<8;j++){ //设置
                    bms.set_correctTemp(j,tCorrectModel.get(j).value3,tCorrectModel.get(j).value2);
                }
                bms.start_correctTemp();
            }
            if(aCorrect){
                aCorrect=false;
                for(var i=0;i<2;i++){
                    if(aCorrectModel.get(i).value3){
                        bms.correctA(i,aCorrectModel.get(i).value2);
                    }
                }
            }
            if(timeCorrect){
                timeCorrect=false;
                bms.getRtcTime();
            }
        }
    }

    Timer{
        id:rtcCerrectTimer
        interval: 1000
        repeat: false
        onTriggered: {
           // console.log("检查串口")
            timeCorrectButton.enabled=true;
        }
    }
    Timer{
        id:tCorrectTimer
        interval: 1000
        repeat: false
        onTriggered: {
           // console.log("检查串口")
            tCorrectButton.enabled=true;
        }
    }
    Timer{
        id:iCorrectTimer
        interval: 1000
        repeat: false
        onTriggered: {
           // console.log("检查串口")
            iCorrectButton.enabled=true;
        }
    }
}
