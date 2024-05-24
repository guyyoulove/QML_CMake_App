import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import an.qt.SerialPortInfo 1.0
import an.qt.BaudInfo 1.0
import an.qt.BMS 1.0
import an.qt.BatteryInfoModel 1.0
import an.qt.TempInfoModel 1.0
import an.qt.LogInfoModel 1.0
import an.qt.OtherInfoModel 1.0
import an.qt.Login 1.0
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3

ApplicationWindow {
    id:bmsApp
    visible: true

    maximumWidth: 1200  //1900

    minimumWidth :1200  //1900
    maximumHeight: 980  //900
    minimumHeight: 980
    title: "BMS"

    color:"#F4F7FC"
    property bool isValid: login.isValid() //是否授权的标志
    property bool isSerialOpen: false  //串口是否打开标志
    property bool isReadBatteryInfo: false //之前是否正在读电池信息，是为true，不是为false
    property bool isReadTempInfo: false //之前是否正在读温度信息，是为true，不是为false
    property bool isHoldNo2_18: false //之前是否得到了no2_18的值

    property int buttonFlag: 0
    property int comErrorCnt: 0   //通讯失败的次数
    property int comSuccessCnt: 0  //通讯成功的次数


    property alias e_x1: x1
    property alias e_x2: x2
    property alias e_x3: x3
    property alias e_x4: x4
    // property var e_otherInfoModel:

    BMS{
        id:bms
    }

    TipsWin{
        id:tipsWin
    }
    Login{
        id:login
    }
    LoginWin{
        id:loginWin
    }

    //    OtherInfoPage{
    //        id:otherInfoPage
    //    }
    SelectTime{
        id:selectTime
    }
    RxTimeRecord{
        id:rxTime
    }

    BatteryInfoModel{
        id:batteryInfoModel
    }
    TempInfoModel{
        id:tempInfoModel
    }

    LogInfoModel{
        id:logInfoModel
    }

    OtherInfoModel{
        id:otherInfoModel
    }

    Component{
        id:dispNo
        Item{
            id:root
            anchors.centerIn: parent
            width:71
            height: 41
            Button{
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                id:btn
                down: {
                    console.log("data2")
                    return getValue2
                }

                onClicked:{
                    if(down){
                        down=false;
                        if(viewName=="删除日志"){
                            console.log(rowNo,pageNo,false,log.showFlag)
                            logInfoModel.recordDeleteLog(rowNo,pageNo,false,log.showFlag);
                        }
                    }
                    else{
                        down=true;
                        if(viewName=="删除日志"){
                            console.log(rowNo,pageNo,true,log.showFlag)
                            logInfoModel.recordDeleteLog(rowNo,pageNo,true,log.showFlag);
                        }
                    }
                }
                background:Image{
                    id:img
                    source: btn.down?"/icon/image2/select.png":"/icon/image2/noselect.png"

                }
                Component.onCompleted: {
                    console.log("状态",down);
                }
            }
            Text{
                anchors.left:btn.right
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                text: getValue2
                font.family: "微软雅黑"
                font.pixelSize: 16
                color:"#666666"
            }
        }

    }

    //----------------------------------------------

    //----------------------------------------------


    //----------------------------------------------

    //----------------------------------------------

    Component{
        id:dispText
        Text{
            text:getValue
            font.family: "微软雅黑"
            font.pixelSize: {
                if("otherTable"==viewName)
                    return 14;
                else
                    return 16;
            }

            horizontalAlignment:Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color:{
                if(colNo===0)
                    return "#666666"
                else
                    return "#333333"
            }
        }
    }

    Component{
        id:dispBal    //未开启时显示灰色，开启后显示红色或绿色
        Item{
            anchors.centerIn: parent
            Image{
                anchors.centerIn: parent
                source:{
                    if(!isSerialOpen){
                        //没开串口
                        return "/icon/image/nobal.png"
                    }
                    else{ //getBoolValue true为开启  false为未开启
                        if(getBoolValue)
                            return "/icon/image/bal.png";
                        else
                            return "/icon/image/nobal.png";
                    }

                }



            }
        }
    }
    Component{  //记录按钮
        id:dispRecord
        Item{
            anchors.centerIn: parent
            Button{
                id:btn
                down:getBoolValue
                onClicked: {
                    if(down){
                        if("batteryTable"==viewName){
                            batteryInfoModel.record(rowNo,false);
                        }
                        else if("tempTable"==viewName){
                            tempInfoModel.record(rowNo,false);
                        }
                        else if("otherTable"==viewName){
                            otherInfoModel.record(rowNo,false);
                        }
                    }
                    else{
                        if("batteryTable"==viewName){
                            batteryInfoModel.record(rowNo,true);
                        }
                        else if("tempTable"==viewName){
                            tempInfoModel.record(rowNo,true);
                        }
                        else if("otherTable"==viewName){
                            otherInfoModel.record(rowNo,true);
                        }
                    }
                }
                anchors.centerIn: parent
                background:Image{
                    source: getBoolValue?"/icon/image/recordy.png":"/icon/image/noscan.png"
                }
            }
        }
    }
    Component{  //扫描按钮
        id:dispScan
        Item{
            anchors.centerIn: parent
            Button{
                id:btn
                down:getBoolValue
                onClicked: {
                    if(down){
                        if("batteryTable"==viewName){
                            batteryInfoModel.scan(rowNo,false);
                        }
                        else if("tempTable"==viewName){
                            tempInfoModel.scan(rowNo,false);
                        }
                        else if("otherTable"==viewName){
                            otherInfoModel.scan(rowNo,false);
                        }
                    }
                    else{
                        if("batteryTable"==viewName){
                            batteryInfoModel.scan(rowNo,true);
                        }
                        else if("tempTable"==viewName){
                            tempInfoModel.scan(rowNo,true);
                        }
                        else if("otherTable"==viewName){
                            otherInfoModel.scan(rowNo,true);
                        }
                    }
                }
                anchors.centerIn: parent
                background:Image{
                    source: getBoolValue?"/icon/image/scan.png":"/icon/image/noscan.png"
                }
            }
        }
    }


    MessageDialog{
        id:msg
        standardButtons : StandardButton.Ok
    }
    Popup{
        id:pop
        property alias text: tips.text
        x:(bmsApp.width-back.width)/2
        background: Rectangle{
            id:back
            color:"#F2F7FE"
            border.color: "#C3C8D0"
            border.width: 1
            radius: 5
            width: 230
            height: 30
        }

        contentItem: Text{
            id:tips
            anchors.centerIn: back
            font.family: "微软雅黑"
            font.pixelSize: 16
            color: "red"
        }

        closePolicy: Popup.NoAutoClose
    }

    FileDialog{
        id:fileDialog
        property string viewName: ""
        onAccepted: {
            if("导出记录"==viewName){
                pop.text=qsTr("正在导出记录文件，请稍后...")
                pop.open();
                bms.export_record(fileUrl);

            }
            else if("导出参数"==viewName)
                bms.exportParam(fileUrl,paramConfig.nameInCapModel,paramConfig.nameInBasicModel,paramConfig.name3InBasicModel,paramConfig.nameInAdvanced,paramConfig.nameInOther,paramConfig.name2InOther);
            else if("导入参数"==viewName)
                bms.importParam(fileUrl);

            else if("导入Afe"==viewName)
                bms.importAfe(fileUrl);
            else if("导出Afe"==viewName)
                 bms.exportAfe(fileUrl,romPage.nameInRomModel,romPage.name2InRomModel,romPage.name3InRomModel);

        }
    }

    SerialPortInfo{
        id:serialPortInfo
    }
    BaudInfo{
        id:baudInfo
    }




    Image {
        x:0
        y:-27
        source: "/icon/image/leftline.jpg"
    }
    Image{
        x:0
        y:-55
        width:200.6
        source: "/icon/image/menuImg.jpg"
        Text{
            x:17.12
            y:11
            text:qsTr("智能BMS管理系统")
            font.family: "微软雅黑"
            font.pixelSize: 14
        }
    }
    menuBar:MenuBar{
        id:menuBar
        x:200.6
        y:0
        background:Image{
            source: "/icon/image/menuImg.jpg"
        }
        spacing: 37
        font.pixelSize: 14
        font.family: "微软雅黑"
        Menu{

            title: qsTr("文件")
            font.family: "微软雅黑"
            font.pixelSize: 16
            implicitWidth :130
            background: Rectangle{
                width: 130
                height:160
                border.width:2
                border.color: "#D6EAFE"

            }

            MenuItem{
                text:qsTr("导出记录参数")
                onClicked: {
                    fileDialog.viewName="导出记录";
                    fileDialog.title=qsTr("导出记录：请选择一个excel文件")
                    fileDialog.nameFilters=["excel files (*xlsx)"];
                    fileDialog.open();
                }
            }
            MenuItem{
                text:qsTr("导入配置参数")
                onClicked: {
                    fileDialog.viewName="导入参数";
                    fileDialog.title=qsTr("导入参数：请选择一个excel文件")
                    fileDialog.nameFilters=["excel files (*xlsx)"];
                    fileDialog.open();
                }
            }
            MenuItem{
                text:qsTr("导出配置参数")
                onClicked: {
                    fileDialog.viewName="导出参数";
                    fileDialog.title=qsTr("导出参数：请选择一个excel文件")
                    fileDialog.nameFilters=["excel files (*xlsx)"];
                    fileDialog.open();
                }
            }
            MenuItem{
                text:qsTr("退出")
            }
        }
        Menu{
            title: qsTr("选项")
            font.family: "微软雅黑"
            font.pixelSize: 16
            implicitWidth :130
            background: Rectangle{
                width: 130
                height:120
                border.width:2
                border.color: "#D6EAFE"

            }
            Menu{
                title:qsTr("语言")
                implicitWidth :130
                background: Rectangle{
                    width: 130
                    height:80
                    border.width:2
                    border.color: "#D6EAFE"

                }
                MenuItem{
                    text:qsTr("简体中文")
                }
                MenuItem{
                    text:qsTr("英文")
                }
            }

            MenuItem{
                text:qsTr("授权")
                onClicked: {
                    loginWin.show();
                }
            }
            MenuItem{
                text:qsTr("软件更新")
            }

        }
        Menu{
            title: qsTr("帮助")
            font.family: "微软雅黑"
            font.pixelSize: 16
            implicitWidth :130
            background: Rectangle{
                width: 130
                height:80
                border.width:2
                border.color: "#D6EAFE"

            }
            MenuItem{
                text:qsTr("帮助文档")
            }
            MenuItem{
                text:qsTr("关于")
            }

        }
    }




    ComDialog{  //整个界面的左边按钮栏
        id:comDialog
    }


    StackLayout {
        id:stack
        x:181
        y:8
        width: 1020  //1720
        height: 806

        currentIndex: 0
        Item {
            id: batteryInfoTab
            BatteryInfo{
                id:batteryInfoPage
            }
        }
        Item {
            id: parameterConfTab
            ParamConfig{
                id:paramConfig
            }
        }
        Item {
            id: correctTab
            Correct{
                id:correct
            }
        }
        Item {
            id: logTab
            LogPage{
                id:log
            }
        }
        Item {
            id: romTab
            RomPage{
                id:romPage
            }
        }
    }

    Row{
        x:181
        y:824
        spacing: 110
        ParamConfigInput{
            e_viewFlag: "延时"
            anchors.verticalCenter: parent.verticalCenter
            id:x1
            e_text: qsTr("等待回复的延时")
            e_recX: 117
            e_input: "50"
            e_unit: "mS"
        }
        ParamConfigInput{
            e_viewFlag: "延时"
            anchors.verticalCenter: parent.verticalCenter
            id:x2
            e_text: qsTr("字节间延时")
            e_recX: 85
            e_input: "30"
            e_unit: "mS"
        }
        ParamConfigInput{
            e_viewFlag: "延时"
            anchors.verticalCenter: parent.verticalCenter
            id:x3
            e_text: qsTr("通讯错误再发送的延时")
            e_recX: 165
            e_input: "200"
            e_unit: "mS"
        }
        ParamConfigInput{
            e_viewFlag: "延时"
            anchors.verticalCenter: parent.verticalCenter
            id:x4
            e_text: qsTr("通讯正确的延时")
            e_recX: 117
            e_input: "1000"
            e_unit: "mS"
        }
    }
    Row{
        x:181
        y:869
        spacing: 120
        ParamConfigInput{
            e_viewFlag: "通讯失败次数"
            anchors.verticalCenter: parent.verticalCenter
            e_text: qsTr("连续通讯失败次数")
            e_recX: 130
            e_input: "20"
            e_unit: "次"
        }
        MyButton{
            anchors.verticalCenter: parent.verticalCenter
            e_btnBackColor:down?"gray":"#00B5AC"
            e_text: down?qsTr("继续通讯"):qsTr("停止通讯")  //按下：继续通讯  不按：停止通讯
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
                if(down){
                    //失败N次后停止通讯
                    down=false;
                    bms.setComFailBool(false);

                }
                else{
                    //失败N次后继续通讯
                    down=true;
                    bms.setComFailBool(true);
                }


            }
        }
    }
    Text{
        id:comStatus
        font.family: "微软雅黑"
        font.pixelSize: 15
        x:10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
    }
    Text{
        id:comError
        font.family: "微软雅黑"
        font.pixelSize: 15
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        color: "red"
        text:"总通讯失败次数"+comErrorCnt
    }
    Text{
        id:comSuccess
        font.family: "微软雅黑"
        font.pixelSize: 15
        anchors.right: comError.left
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        color: "green"
        text:"总通讯成功次数"+comSuccessCnt
    }
    property int indexList: [0,3,6,9,12,15,18]
    property var tapContent
    Connections{
        target: bms
        onComSleep:{
            console.log("onComSleepxx",isReadBatteryInfo,isReadTempInfo);
            if(isReadBatteryInfo) //读电池信息
                bms.readBatteryInfo(!isHoldNo2_18);
            if(isReadTempInfo) //读温度信息
                bms.correct_readTemp();
        }

        onResponseMsg:{
            if("打开串口成功"==s){
                console.log("打开串口成功！")
                batteryInfoPage.e_allRecordBtn=true;
                batteryInfoPage.e_allScanBtn=true;

                batteryInfoPage.e_allRecordTempBtn=true;
                batteryInfoPage.e_allScanTempBtn=true;

                batteryInfoPage.e_allRecordOtherBtn=true;
                batteryInfoPage.e_allScanOtherBtn=true;
                //串口已经打开
                if(0==stack.currentIndex){  //串口在电池信息界面打开
                    isSerialOpen=true;
                    isReadBatteryInfo=true;
                    isReadTempInfo=false;
                    comStatus.text="";
                    console.log("isReadBatteryInfo",isReadBatteryInfo);
                }
                else if(2==stack.currentIndex){  //串口在校准界面打开
                    isSerialOpen=true;
                    isReadTempInfo=true;
                    isReadBatteryInfo=false;
                    comStatus.text="";
                    console.log("isReadTempInfo",isReadTempInfo);
                }
                else{
                    //串口在其他页打开
                    isSerialOpen=true;
                    isReadTempInfo=false;
                    isReadBatteryInfo=false;
                    comStatus.text="";
                }
            }

            else if("关闭串口成功"==s){
                //串口已经关闭
                isSerialOpen=false;
                isReadBatteryInfo=false;
                isReadTempInfo=false;
                isHoldNo2_18=false;
                comErrorCnt=0;
                comSuccessCnt=0;
            }
            else if("通讯失败！没有数据回复或通讯错误"==s){//通讯发生了错误，通讯线程暂停，串口依然打开
                console.log("通讯失败！没有数据回复或通讯错误");
                isReadBatteryInfo=false;
                isReadTempInfo=false;
                comStatus.text="通讯失败，检查后请重新打开串口";
                comStatus.color="red";
                msg.text=s;
                msg.open();
            }
            else if("发送两次均无回复！"==s){
                comStatus.text=s;
                comStatus.color="red";
            }

            else{
                if(isDialog){
                    msg.text=s;
                    msg.open();
                }

                else{
                    //在状态栏显示
                    comStatus.text=s;
                    comStatus.color="red";
                    comErrorCnt++;
                }
            }

        }

        onExportRecordStatus:{
            pop.close();
            msg.text=s;
            msg.open();

        }

        onGetAFE_S:{
            //放电过流2 电压
            if(s=="TI-76930"){
                console.log("TI-76930");
                tapContent=["17mv","22mv","28mv","33mv","39mv","44mv","50mv","56mv","61mv",
                            "67mv","72mv","78mv","83mv","89mv","94mv","100mv"];
            }
            else if(s=="中颖-367309"){
                tapContent=["30mv","40mv","50mv","60mv","70mv","80mv","90mv","100mv","120mv",
                            "140mv","160mv","180mv","200mv","300mv","500mv"];

            }
            paramConfig.e_fdgl2_1_content=tapContent;

            //短路保护 电压
            if(s=="TI-76930"){
                tapContent=["44mv","67mv","89mv","111mv","133mv","155mv","178mv","200mv"];
            }
            else if(s=="中颖-367309"){
                tapContent=["50mv","80mv","110mv","140mv","170mv","200mv","230mv","260mv","290mv",
                            "320mv","350mv","400mv","500mv","600mv","800mv","1000mv"];

            }
            paramConfig.e_short_1_content=tapContent;

            //放电过流2 延时
            if(s=="TI-76930"){
                tapContent=["8ms","20ms","40ms","80ms","160ms","320ms","640ms","1280ms"];
            }
            else if(s=="中颖-367309"){
                tapContent=["10ms","20ms","40ms","60ms","80ms","100ms","200ms","400ms","600ms",
                            "800ms","1s","2s","4s","8s","10s","20s"];

            }
            paramConfig.e_fdgl2_1_delay_content=tapContent;

            // 第二行短路保护延时
            if(s=="TI-76930"){
                tapContent=["70us","100us","200us","400us"];
            }
            else if(s=="中颖-367309"){
                tapContent=["0us","64us","128us","192us","256us","420us","384us","448us","512us",
                            "576us","640us","704us","768us","832us","896us","960us"];

            }
            paramConfig.e_short_1_delay_content=tapContent;

        }

        onNo145_147: {
            console.log("onNo145_147",v1,v2,v3)

            comDialog.e_power=v1;
            var j=0;
            for(var i=0;i<16;i++){
                j=v2&(1<<i);
                batteryInfoPage.e_bmsStatusModel.setProperty(i,"unit",j)
            }
            for(i=16;i<29;i++){
                j=v3&(1<<(i-16));
                batteryInfoPage.e_bmsStatusModel.setProperty(i,"unit",j)
            }

        }
        onNo2_15:{
            console.log("onNo2_15")
            isHoldNo2_18=true;
            batteryInfoPage.e_dataModel.setProperty(0,"value",v1);
            batteryInfoPage.e_dataModel.setProperty(2,"value",v2);
            batteryInfoPage.e_dataModel.setProperty(0,"value3",v3);
            batteryInfoPage.e_dataModel.setProperty(1,"value3",v4);
            batteryInfoPage.e_dataModel.setProperty(2,"value3",v5);
            batteryInfoPage.e_dataModel.setProperty(2,"value2",v6);
            batteryInfoPage.e_dataModel.setProperty(0,"value2",v7);
        }

        onNo192_194:{
            console.log("onNo192_194")
            batteryInfoPage.e_mos_d=v1;
            batteryInfoPage.e_mos_c=v2;
            batteryInfoPage.e_mos_p=v3;
            batteryInfoPage.e_dataModel.setProperty(1,"value",v4+" kΩ , "+v5+" kΩ");
        }

        onLoadExcelFail:{
            msg.text="加载参数配置文件出错！请检查文件是否在错误路径";
            msg.open();
        }
        onParamInCap:{
            console.log(index,param);
            paramConfig.e_capModel.setProperty(index, "value", param);

        }
        onParamInBasic1:{
            console.log(index,param);
            paramConfig.e_basicModel.setProperty(index, "value", param.toFixed(1));
        }

        onParamInBasic2:{
            paramConfig.e_basicModel.setProperty(index, "value2", param);
        }
        onParamInBasic3:{
            paramConfig.e_basicModel.setProperty(index, "value3", param.toFixed(1));
        }
        onParamInAdvanced:{
            paramConfig.e_advancedModel.setProperty(index, "value", param);
        }
        onParamInAdvanced2:{
            paramConfig.e_advancedModel.setProperty(index, "value2", param);
        }
        onParamInOther:{
            paramConfig.e_otherModel.setProperty(index, "value", param);
        }
        onParamInOther2:{
            paramConfig.e_otherModel.setProperty(index, "value2", param);
        }
        onParamInResistor:{
            paramConfig.e_resistor=param.toFixed(1);
        }
        onParamInCharge:{
            if(0==index)
                paramConfig.e_chargeModel.setProperty(index, "value", param.toFixed(2));
            else
                paramConfig.e_chargeModel.setProperty(index, "value", param);
        }

        onIsWriteParamOk:{
            if(status){
                comStatus.color="black";
                comStatus.text="写参数成功！"
            }
            else
            {
                comStatus.color="red";
                comStatus.text="写参数失败！"
            }
        }
        //void getCorrectTemp(qint16 value,quint8 num);//value值 num代表哪一个温度值

        onGetCorrectTemp:{  //num从1开始  校准成功  emit getCorrectTemp(temp,m_comFlag-10); value num
            console.log("getcorrectTemp",value,num-1);
            correct.e_tCorrectModel.setProperty(num-1, "value", value);
            correct.e_tCorrectModel.setProperty(num-1, "value4", true);
        }
        onGetCorrectRealTemp:{
            isReadTempInfo=true;
            correct.e_tCorrectModel.setProperty(index, "value",value);
            correct.e_tCorrectModel.setProperty(index, "value4", false);

        }

        //emit no141(v*0.01);
        onNo141:{
            correct.e_aCorrectModel.setProperty(0, "value", value.toFixed(2));
            correct.e_aCorrectModel.setProperty(1, "value", value.toFixed(2));
            console.log("电流校准",correct.aCorrectFlag)
            //            if(correct.aCorrectFlag>0 && correct.e_aCorrectModel.get(correct.aCorrectFlag-1).value2String!="输入" && correct.e_aCorrectModel.get(correct.aCorrectFlag-1).value2String!=""){
            //                console.log("电流校准",correct.e_aCorrectModel.get(correct.aCorrectFlag-1).value2)
            //                bms.correctA(correct.aCorrectFlag,correct.e_aCorrectModel.get(correct.aCorrectFlag-1).value2);
            //            }
            //            else{
            //                msg.text=qsTr("无已勾选校准项或校准数值输入不正确，无法校准，请修改！")
            //                msg.open();
            //            }
        }
        onGetCorrectA:{
            if(0==num)
                correct.e_aCorrectModel.setProperty(3, "value", value);
            else if(1==num){
                correct.e_aCorrectModel.setProperty(2, "value", (value*0.001).toFixed(3));
            }
            isReadTempInfo=true;
            bms.correct_readTemp();

        }
        onRtcTime:{ //校准页 rtc校准
            correct.e_rtcTime=s;
            isReadTempInfo=true;
            bms.correct_readTemp();
        }

        onGetCorrectControl:{ //quint16 value
            batteryInfoPage.e_controlIn=value;
            isReadBatteryInfo=true;
            bms.readBatteryInfo(!isHoldNo2_18);
        }
        onGetCorrectMos:{ //quint16 value
            batteryInfoPage.e_mos=value;
            if(0==value){
                batteryInfoPage.e_mosc_down=false; //充
                batteryInfoPage.e_mosd_down=false; //放
            }
            else if(1==value){
                batteryInfoPage.e_mosc_down=true; //充
                batteryInfoPage.e_mosd_down=false; //放
            }
            else if(2==value){
                batteryInfoPage.e_mosc_down=false; //充
                batteryInfoPage.e_mosd_down=true; //放
            }
            else if(3==value){
                batteryInfoPage.e_mosc_down=true; //充
                batteryInfoPage.e_mosd_down=true; //放
            }
            isReadBatteryInfo=true;
            bms.readBatteryInfo(!isHoldNo2_18);
        }
        //responseTime(s);
        onComSuccessfully:{
            comSuccessCnt++;
        }

//        onResponseTime:{
//            rxTime.e_model.append({"value": s})

//        }

        onNo101_109:{  //v值 index行   index:0-8
            console.log("no101_109",index)
            if(index<5)
                log.e_historyModel.setProperty(index,"value",v);
            else
                log.e_historyModel.setProperty(index-5,"value2",v);
        }
        onNo110_113:{  //v值 index行   index:9-12
            console.log("no110_113",index)
            log.e_historyModel.setProperty(index-9,"value3",v);
        }
        onNo114_115:{  //v值 index行  index:13-14
            console.log("no114_115",index)
            if(index==13)
                log.e_historyModel.setProperty(4,"value2",(v*0.01).toFixed(2));
            else
                log.e_historyModel.setProperty(4,"value3",(v*0.01).toFixed(2));
        }



        onDeleteROMLog:{
            if(b){
                //删除成功
                msg.text=qsTr("删除ROM日志成功");
                msg.open();
            }
            else{
                //删除失败
                msg.text=qsTr("删除ROM日志失败");
                msg.open();
            }
        }

        onValueInAfe:{
            if(index<10)
                romPage.e_romModel.setProperty(index, "value", value.toString());
            else if(index<20)
                romPage.e_romModel.setProperty(index-10, "value2", value.toString());
            else
                romPage.e_romModel.setProperty(index-20, "value3", value.toString());
        }
        onValueInAfeHex:{
            if(index<10)
                romPage.e_romModel.setProperty(index, "value", value);
            else
                romPage.e_romModel.setProperty(index-20, "value3", value);
        }
    }



}
