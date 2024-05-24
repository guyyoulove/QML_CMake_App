import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.1
import QtQml.Models 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.5
import QtQuick 2.7
ScrollView { //参数配置页面
    width: 1020 //1720
    height: 806
    contentWidth:1000 //1700
    contentHeight:1350
    //contentHeight:1006
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    //ScrollBar.vertical.policy: ScrollBar.AlwaysOff
    clip: true
    //电池信息界面
    property alias e_bmsStatusModel: bmsStatusModel
    property alias e_dataModel: dataModel
    property alias e_mos_d: mos_d.e_isOpen
    property alias e_mos_c: mos_c.e_isOpen
    property alias e_mos_p: mos_p.e_isOpen
    property alias e_controlIn: controlIn.e_input

    property alias e_allRecordBtn:allRecordBtn.down
    property alias e_allScanBtn:allScanBtn.down
    property alias e_allRecordTempBtn: allRecordTempBtn.down
    property alias e_allScanTempBtn:allScanTempBtn.down
    property alias e_allRecordOtherBtn: allRecordOtherBtn.down
    property alias e_allScanOtherBtn:allScanOtherBtn.down




    property bool controlCorrect: false //是否在进行控制命令校准 是为true  不是为false
    property bool mosCorrect: false //是否在进行mos命令 是为true  不是为false
    property int e_v: -1
    property int e_mos :-1
    property alias e_mosd_down:mos_d.e_btnDown
    property alias e_mosc_down:mos_c.e_btnDown

    Row{  //管子
        x:0
        y:0
        spacing: 17

        No192{
            id:mos_p
            e_img:(e_isOpen && isSerialOpen)? "/icon/image/predischarge.png":"/icon/image2/gray.png"
            e_name:qsTr("预放管")

        }
        No192{
            id:mos_d
            e_img:(e_isOpen && isSerialOpen)? "/icon/image/discharge.png":"/icon/image2/gray.png"
            e_name:qsTr("放电管")
            onE_downChanged: { //e_down false 解除  true  关断
                console.log(e_down)
                if(!e_down){  //放电管 解除
                    if(!mos_c.e_down){
                        //两个都解除 发0
                        e_v=0;
                    }
                    else{
                        //放电管 解除 充电管 关断 发 1
                         e_v=1;
                    }

                }
                else{ //放电管 关断
                    if(mos_c.e_down){
                        //两个都关断 发3
                         e_v=3;
                    }
                    else{
                        //放电管 关断 充电管 解除 发 2
                         e_v=2;
                    }

                }
                if(isSerialOpen){  //串口已打开
                    //停止读实时温度和实时电流
                    isReadBatteryInfo=false;
                    isReadTempInfo=false;
                    bms.stopReadInfo();
                    mosCorrect=true;

                }
                else{
                    msg.text=qsTr("未打开串口！");
                    msg.open();
                }
            }
        }
        No192{
            id:mos_c
            e_img:(e_isOpen && isSerialOpen)? "/icon/image/charge.png":"/icon/image2/gray.png"
            e_name:qsTr("充电管")
            onE_downChanged: { //e_down false 解除  true  关断
                console.log(e_down)
                if(!e_down){  //充电管 解除
                    if(!mos_d.e_down){
                        //两个都解除 发0
                         e_v=0;
                    }
                    else{
                        //充电管 解除 放电管 关断 发 2
                         e_v=2;
                    }

                }
                else{ //充电管 关断
                    if(mos_d.e_down){
                        //两个都关断 发3
                         e_v=3;
                    }
                    else{
                        //充电管 关断 放电管 解除 发 1
                         e_v=1;
                    }

                }
                if(isSerialOpen){  //串口已打开
                    //停止读实时温度和实时电流
                    isReadBatteryInfo=false;
                    isReadTempInfo=false;
                    bms.stopReadInfo();
                    mosCorrect=true;

                }
                else{
                    msg.text=qsTr("未打开串口！");
                    msg.open();
                }
            }
        }



    }
//    Text{
//        x:0
//        y:0
//        text:"e_mos:"+e_mos+" e_v"+e_v
//    }
    Image{  //扫描
        x:574
        y:0
        source: "/icon/image/scantimeback.jpg"
        Column{
            id:col
            x:27
            y:31
            spacing: 21
            ScanAndRecordTimeBlock{
                id:scanTime
                text:qsTr("扫描时间设定")
            }
            ScanAndRecordTimeBlock{
                id:recordTime
                text:qsTr("记录时间间隔")
            }
            ScanBlock{
                id:controlIn
                text:qsTr("控制命令")
                e_unit: qsTr("HEX")
                e_unitLocation: 152
                MyButton{
                    id:btn
                    x:296
                    enabled: (controlIn.e_input=="单行输入" || controlIn.e_input=="")? false:true
                    anchors.verticalCenter: parent.verticalCenter
                    e_btnBackW: 75
                    e_btnBackH: 35
                    e_btnBackColor: "#00B5AC"
                    e_text: qsTr("导出")
                    onClicked:{
                        if(isSerialOpen){  //串口已打开
                            //停止读实时温度和实时电流
                            isReadBatteryInfo=false;
                            isReadTempInfo=false;
                            controlCorrect=true;
                            bms.stopReadInfo();
                            cmdTimer.start();
                            btn.enabled=false;

                        }
                        else{
                            msg.text=qsTr("未打开串口！");
                            msg.open();
                        }
                    }
                }
            }
        }
        //--------------------------------------------------
    }
    Image{  //电池信息
        id:batteryInfo
        x:0
        y:199
        source: "/icon/image/infoback.jpg"
        Text{
            id:batteryTitle
            x:26
            y:33
            text:{
                if(!isSerialOpen){
                    //没开串口
                    return qsTr("电芯串数信息 --")
                }
                else{
                    return qsTr("电芯串数信息 ")+vTable.rowCount
                }
            }

            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }
        Row{
            x:180
            anchors.verticalCenter: batteryTitle.verticalCenter
            spacing: 10
            Text {
                anchors.verticalCenter:parent.verticalCenter
                text: qsTr("记录全选")
                font.family: "微软雅黑"
                font.pixelSize: 13
                color:"#666666"
            }
            Button{  //电池信息全记录
                id:allRecordBtn
                anchors.verticalCenter:parent.verticalCenter
                down:true
                onClicked: {
                    if(down){
                        down=false;
                        batteryInfoModel.allRecord(false);
                    }
                    else{
                        down=true;
                        batteryInfoModel.allRecord(true);
                    }
                }
                background:Image{
                    source:allRecordBtn.down?"/icon/image/recordy.png":"/icon/image/noscan.png"
                }
            }
            Text {
                anchors.verticalCenter:parent.verticalCenter
                text: qsTr("扫描全选")
                font.family: "微软雅黑"
                font.pixelSize: 13
                color:"#666666"
            }
            Button{  //电池信息全扫描
                id:allScanBtn
                anchors.verticalCenter:parent.verticalCenter
                down:true
                onClicked: {
                    if(down){
                        down=false;
                        batteryInfoModel.allScan(false);
                    }
                    else{
                        down=true;
                        batteryInfoModel.allScan(true);
                    }
                }
                background:Image{
                    source:allScanBtn.down?"/icon/image/scan.png":"/icon/image/noscan.png"
                }
            }
        }
        //        Button{
        //            x:252
        //            anchors.verticalCenter: name.verticalCenter
        //            onClicked:{
        //                if(isVFlag){ //显示温度
        //                    isVFlag=0;
        //                    vTable.visible=false;
        //                    tempTable.visible=true;
        //                }
        //                else{ //显示电压
        //                    isVFlag=1;
        //                    vTable.visible=true;
        //                    tempTable.visible=false;
        //                }
        //            }

        //            background: Image{
        //                source: "/icon/image/iconswitch.png"
        //            }
        //        }
        //        Text{
        //            x:273
        //            anchors.verticalCenter: name.verticalCenter
        //            text:{
        //                if(isVFlag)
        //                    return qsTr("电池温度")
        //                else
        //                    return qsTr("电池电压")
        //            }
        //            font.family: "微软雅黑"
        //            font.pixelSize: 16
        //            color:"#1AB2A1"
        //        }
        ScrollView { //电池信息
            x:27
            y:66
            width: 338
            height:520
            contentWidth:320
            contentHeight:1320
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            clip: true
            TableView{
                id:vTable
                x:0
                y:0
                width:320
                height:1320
                backgroundVisible: false
                frameVisible: false
                horizontalScrollBarPolicy:Qt.ScrollBarAlwaysOff
                verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                visible: true
                clip: true
                headerDelegate:Rectangle{
                    height: 40
                    Text{
                        anchors.centerIn: parent
                        text:styleData.value
                        font.family: "微软雅黑"
                        font.pixelSize: 16
                        color:"#666666"
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                itemDelegate:Loader{
                    property var getValue: 0
                    property bool getBoolValue: false
                    property var colNo:styleData.column
                    property string viewName: "batteryTable"
                    property int rowNo: styleData.row
                    sourceComponent: {
                        if(styleData.column===2)
                        {
                            getBoolValue=styleData.value
                            return dispBal
                        }
                        else if(styleData.column===3)
                        {
                            getBoolValue=styleData.value
                            return dispRecord
                        }
                        else if(styleData.column===4)
                        {
                            getBoolValue=styleData.value
                            return dispScan
                        }
                        else if(styleData.column===0)
                        {
                            getValue=styleData.value
                            return dispText
                        }
                        else{
                            if(!isSerialOpen){
                                //没开串口
                                getValue="-- --"
                            }
                            else{
                                getValue=styleData.value.toString()
                            }
                            return dispText
                        }
                    }
                }
                rowDelegate: Rectangle{
                    width:310
                    height:40
                    color:"white"
                    Canvas{
                        x:0
                        y:0
                        width:310
                        height:40
                        onPaint: {
                            var ctx = getContext("2d");
                            ctx.fillStyle = "#EFF3F8"
                            ctx.fillRect(0,39, width, 1);
                        }
                    }

                }
                TableViewColumn {
                    role: "nameRole"
                    title:qsTr("名称")
                    width: 47
                }
                TableViewColumn {
                    role: "vRole"
                    title: qsTr("数值/mV")
                    width: 94
                }

                TableViewColumn {
                    role: "balRole"
                    title:qsTr("均衡")
                    width: 61
                }
                TableViewColumn {
                    role: "recordRole"
                    title: qsTr("记录")
                    width: 61
                }
                TableViewColumn {
                    role: "scanRole"
                    title: qsTr("扫描")
                    width: 47
                }
                model: batteryInfoModel
                Component.onCompleted: {
                    bms.setBatteryInfoModel(batteryInfoModel)
                }
            }
        }


    }
    //----------------温度信息----------------------------

    Image{  //温度信息
        x:0
        y:817
        //        id:temp
        //        anchors.left: batteryInfo.right
        //        anchors.leftMargin: 10
        //        anchors.top: batteryInfo.top
        source: "/icon/image2/tempInfo.jpg"
        Text{
            id:tempTitle
            x:26
            y:33
            text:{
                if(!isSerialOpen){
                    //没开串口
                    return qsTr("电芯温度信息 --")
                }
                else{
                    return qsTr("电芯温度信息 ")+tempTable.rowCount
                }
            }

            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }
        Row{
            x:180
            anchors.verticalCenter: tempTitle.verticalCenter
            spacing: 10
            Text {
                anchors.verticalCenter:parent.verticalCenter
                text: qsTr("记录全选")
                font.family: "微软雅黑"
                font.pixelSize: 13
                color:"#666666"
            }
            Button{  //温度信息全记录
                id:allRecordTempBtn
                anchors.verticalCenter:parent.verticalCenter
                down:true
                onClicked: {
                    if(down){
                        down=false;
                        tempInfoModel.allRecord(false);
                    }
                    else{
                        down=true;
                        tempInfoModel.allRecord(true);
                    }
                }
                background:Image{
                    source:allRecordTempBtn.down?"/icon/image/recordy.png":"/icon/image/noscan.png"
                }
            }
            Text {
                anchors.verticalCenter:parent.verticalCenter
                text: qsTr("扫描全选")
                font.family: "微软雅黑"
                font.pixelSize: 13
                color:"#666666"
            }
            Button{  //温度信息全扫描
                id:allScanTempBtn
                anchors.verticalCenter:parent.verticalCenter
                down:true
                onClicked: {
                    if(down){
                        down=false;
                        tempInfoModel.allScan(false);
                    }
                    else{
                        down=true;
                        tempInfoModel.allScan(true);
                    }
                }
                background:Image{
                    source:allScanTempBtn.down?"/icon/image/scan.png":"/icon/image/noscan.png"
                }
            }
        }
        TableView{
            id:tempTable
            x:27
            y:66
            width: 320
            height:360
            backgroundVisible: false
            frameVisible: false
            horizontalScrollBarPolicy:Qt.ScrollBarAlwaysOff
            verticalScrollBarPolicy:Qt.ScrollBarAlwaysOff
            clip: false
            headerDelegate:Rectangle{
                height: 40
                Text{
                    anchors.centerIn: parent
                    text:styleData.value
                    font.family: "微软雅黑"
                    font.pixelSize: 16
                    color:"#666666"
                    horizontalAlignment:Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            itemDelegate:Loader{
                property string getValue:""
                property var colNo:styleData.column
                property bool getBoolValue: false
                property string viewName: "tempTable"
                property int rowNo: styleData.row
                sourceComponent: {
                    if(styleData.column===2)
                    {
                        getBoolValue=styleData.value
                        return dispRecord
                    }
                    else if(styleData.column===3)
                    {
                        getBoolValue=styleData.value
                        return dispScan
                    }
                    else if(styleData.column===0)
                    {
                        getValue=styleData.value
                        return dispText
                    }
                    else{
                        if(!isSerialOpen){
                            //没开串口
                            getValue="-- --"
                        }
                        else{
                            getValue=styleData.value
                        }
                        return dispText
                    }
                }
            }



            rowDelegate: Rectangle{
                width:310
                height:40
                color:"white"
                Canvas{
                    x:0
                    y:0
                    width:310
                    height:40
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.fillStyle = "#EFF3F8"
                        ctx.fillRect(0,39, width, 1);
                    }
                }

            }

            TableViewColumn {
                role: "nameRole"
                title:qsTr("名称")
                width: 47
            }
            TableViewColumn {
                role: "tempRole"
                title: qsTr("数值/℃")
                width: 155
            }

            TableViewColumn {
                role:"recordRole"
                title: qsTr("记录")
                width: 61
            }
            TableViewColumn {
                role:"scanRole"
                title: qsTr("扫描")
                width: 47
            }
            model: tempInfoModel
            Component.onCompleted: {
                bms.setTempInfoModel(tempInfoModel)
            }
        }
    }


    //--------------------------------------------
    Image{ //BMS状态表
        x:383
        y:199
        //        id:bmsStatus
        //        anchors.left: other.right
        //        anchors.leftMargin: 10
        //        anchors.top: temp.top
        source: "/icon/image/statusback.jpg"

        GridView{
            x:28
            y:29
            width:562
            height:324
            cellHeight: 36
            cellWidth:187

            snapMode: GridView.NoSnap
            interactive: false

            delegate: Rectangle{
                width:187
                height: 36
                Text{
                    x:0
                    text:name
                    font.family: "微软雅黑"
                    font.pixelSize:14
                    color:"#333333"
                    anchors.verticalCenter: parent.verticalCenter
                    Image{
                        x:155
                        anchors.verticalCenter: parent.verticalCenter
                        source:{
                            if(!isSerialOpen){  //没开串口
                                return "/icon/image/nobal.png";
                            }
                            else{ //串口已开
                                if(unit===0)
                                    return "/icon/image/error.png";
                                else
                                    return "/icon/image/bal.png";
                            }
                        }

                    }
                }
            }
            model:bmsStatusModel
        }
        ListModel{
            id:bmsStatusModel
        }
        Text{
            text:qsTr("BMS状态表")
            x:26
            y:11
            font.family: "微软雅黑"
            font.pixelSize: 14
            color:"#040404"
        }
        Canvas{
            x:0
            y:0
            width:617
            height: 397
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<9;i++)
                    ctx.fillRect(28,65+i*36,562, 1);
            }
        }

        Canvas{
            x:0
            y:0
            width:617
            height: 397
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<2;i++)
                    ctx.fillRect(205+i*187,33,1, 367);
            }
        }

    }
    //---------------------------------------------------
    Image{ //电池数据
        x:383
        y:613
        //        anchors.left: bmsStatus.left
        //        anchors.top:bmsStatus.bottom
        //        anchors.topMargin: 10
        source: "/icon/image/databack.jpg"
        Text{
            x:26
            y:32
            text:qsTr("电池数据")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }
        //        Button{
        //            x:489
        //            y:24
        //            onClicked:{
        //                otherInfoPage.show();
        //            }

        //            background: Image{
        //                source:"/icon/image/otherbtn.jpg"
        //                Text{
        //                    anchors.centerIn: parent
        //                    text:qsTr("其他信息>")
        //                    font.family: "微软雅黑"
        //                    font.pixelSize: 16
        //                    color:"#00B5AC"
        //                }
        //            }
        //        }

        ListView{
            id:dataList
            x:5
            y:65
            width:607
            height:123
            model:dataModel

            delegate: Control{
                x:0
                y:0
                width:607
                height: 41
                onHoveredChanged: {
                    if(hovered)
                        dataRec.color="#D6EAFE"
                    else
                        dataRec.color="white"
                }

                Rectangle{
                    anchors.fill: parent
                    id:dataRec

                    Text{
                        id:nameCol0
                        x:21
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "微软雅黑"
                        font.pixelSize: 16
                        text: name
                        color:"#666666"
                    }
                    Text{
                        anchors.left: nameCol0.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "微软雅黑"
                        font.pixelSize: 16
                        text: {
                            if(!isSerialOpen){
                                //没开串口
                                if(index==0)
                                    return "-- --";
                                else if(index==1)
                                    return "-- --，-- --";
                                else
                                    return "-- -- --";
                            }
                            else{
                                //开了串口
                                return value;
                            }
                        }
                        color:"#666666"
                    }
                    Text{
                        id:nameCol1
                        x:256
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "微软雅黑"
                        font.pixelSize: 16
                        text: name2
                        color:"#666666"
                    }
                    Text{
                        anchors.left: nameCol1.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "微软雅黑"
                        font.pixelSize: 16
                        text: {
                            if(1==index)
                                return ""
                            else{
                                if(!isSerialOpen){
                                    return "-- --"
                                }
                                else
                                    return value2;
                            }
                        }
                        color:"#666666"
                    }

                    Text{
                        x:425
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "微软雅黑"
                        font.pixelSize: 16
                        text: name3
                        color:"#666666"
                    }
                    Text{
                        x:507
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "微软雅黑"
                        font.pixelSize: 16
                        text: {
                            if(!isSerialOpen){
                                return "-- --"
                            }
                            else
                                return value3;

                        }
                        color:"#666666"
                    }

                }
            }

        }



        ListModel{
            id:dataModel
        }

        Canvas{
            x:0
            y:0
            width:617
            height: 193
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                ctx.fillRect(27,106,562, 1);
                ctx.fillRect(27,147,562, 1);
                ctx.fillRect(408,66,1, 123);
                ctx.fillRect(246,148,1,41);
                ctx.fillRect(246,66,1,41);
            }
        }

    }


    Image{  //电池其他信息
        x:383
        y:817
        //        id:other
        //        anchors.left: temp.left
        //        anchors.top: temp.bottom
        //        anchors.topMargin: 10
        source: "icon/image2/otherback2.jpg"
        Text{
            id:otherInfoTitle
            x:26
            y:22
            text:qsTr("电池其他信息")
            color:"#040404"
            font.family: "微软雅黑"
            font.pixelSize: 16
        }
        Row{
            x:200
            anchors.verticalCenter: otherInfoTitle.verticalCenter
            spacing: 10
            Text {
                anchors.verticalCenter:parent.verticalCenter
                text: qsTr("记录全选")
                font.family: "微软雅黑"
                font.pixelSize: 13
                color:"#666666"
            }
            Button{  //其他信息全记录
                id:allRecordOtherBtn
                anchors.verticalCenter:parent.verticalCenter
                down:true
                onClicked: {
                    if(down){
                        down=false;
                        otherInfoModel.allRecord(false);
                    }
                    else{
                        down=true;
                        otherInfoModel.allRecord(true);
                    }
                }
                background:Image{
                    source:allRecordOtherBtn.down?"/icon/image/recordy.png":"/icon/image/noscan.png"
                }
            }
            Text {
                anchors.verticalCenter:parent.verticalCenter
                text: qsTr("扫描全选")
                font.family: "微软雅黑"
                font.pixelSize: 13
                color:"#666666"
            }
            Button{  //其他信息全扫描
                id:allScanOtherBtn
                anchors.verticalCenter:parent.verticalCenter
                down:true
                onClicked: {
                    if(down){
                        down=false;
                        otherInfoModel.allScan(false);
                    }
                    else{
                        down=true;
                        otherInfoModel.allScan(true);
                    }
                }
                background:Image{
                    source:allScanOtherBtn.down?"/icon/image/scan.png":"/icon/image/noscan.png"
                }
            }
        }
        TableView{
            id:otherInfoTable
            x:27
            y:46
            width:352
            height:470
            backgroundVisible: false
            frameVisible: false
            horizontalScrollBarPolicy:Qt.ScrollBarAlwaysOff
            headerDelegate:Text{
                height:35
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
                property bool getBoolValue: false
                property string viewName: "otherTable"
                property int rowNo: styleData.row
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
                        getBoolValue=styleData.value;
                        return dispRecord
                    }
                    else if(styleData.column===4)
                    {
                        getBoolValue=styleData.value;
                        return dispScan
                    }

                }
            }

            rowDelegate: Rectangle{
                width:342
                height:36
                color:"white"
                Canvas{
                    x:0
                    y:0
                    width:342
                    height:36
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.fillStyle = "#EFF3F8"
                        ctx.fillRect(0,35, width, 1);
                    }
                }

            }

            TableViewColumn {
                role: "nameRole"
                title:qsTr("名称")
                width: 119
            }
            TableViewColumn {
                role: "vRole"
                title: qsTr("数值")
                width: 91
            }

            TableViewColumn {
                role: "unitRole"
                title:qsTr("单位")
                width: 50
            }
            TableViewColumn {
                role:"recordRole"
                title: qsTr("记录")
                width: 50
            }
            TableViewColumn {
                role:"scanRole"
                title: qsTr("扫描")
                width: 50
            }
            model: otherInfoModel
            Component.onCompleted: {
                bms.setOtherInfoModel(otherInfoModel)
            }

        }


    }

    Timer{
        id:cmdTimer
        interval: 1000
        repeat: false
        onTriggered: {
            btn.enabled=true;
        }
    }

    property var nameInBmsStatusModel:["空载","放电","充电","过充保护","过放保护","充电过流保护","放电过流保护","充电过温保护",
        "充电低温保护","放电过温保护","放电低温保护","均衡电路过热保护","板载功率MOS过热保护","NTC故障","模拟前端A失效",
        "模拟前端B失效","电量计芯片失效","电池包进水保护","电池包短路保护","电池开路","电芯间压差过大",
        "绝缘电阻故障","均衡MOS物理短路失效","放电MOS物理短路失效","充电MOS物理短路失效",
        "PACK包并机故障","预放电失败故障","禁止充电故障","AFE电芯监测线剪线"]

    property var nameInDataModel:["SN码","正、负极绝缘电阻","生产日期"]
    property var name2InDataModel:["Pack厂家","","电芯厂家"]
    property var name3InDataModel:["硬件平台","硬件版本","软件版本"]
    Component.onCompleted: {
        for(var i=0;i<29;i++){
            bmsStatusModel.append({name:nameInBmsStatusModel[i],unit:0})
        }
        for(i=0;i<3;i++){
            dataModel.append({name:nameInDataModel[i],name2:name2InDataModel[i],name3:name3InDataModel[i],
                                 value:"",value2:"",value3:""})
        }
    }

    Connections{
        target: batteryInfoModel
        onIsAllRecord:{
            allRecordBtn.down=value;
        }
        onIsAllScan:{
            allScanBtn.down=value;
        }
    }
    Connections{
        target: tempInfoModel
        onIsAllRecord:{
            allRecordTempBtn.down=value;
        }
        onIsAllScan:{
            allScanTempBtn.down=value;
        }
    }
    Connections{
        target: otherInfoModel
        onIsAllRecord:{
            allRecordOtherBtn.down=value;
        }
        onIsAllScan:{
            allScanOtherBtn.down=value;
        }
    }
    Connections{
        target: bms
        onComSleep:{
            if(controlCorrect){
                controlCorrect=false;
                bms.correctControl(controlIn.e_input);
            }
            if(mosCorrect){
                mosCorrect=false;
                bms.correctMos(e_v);
            }
        }
    }
}




