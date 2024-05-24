import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
ScrollView { //日志页面
    property int pageCnt: 0
    property int logCnt: 0

    property int showFlag: 0  //为0显示的是所有日志，为1显示的是筛选的日志

    property alias e_historyModel: historyModel
    //property alias e_rtcTime: rtcTime.e_input

    property int startYear: 0
    property int stopYear: 0
    property int startMon: 0
    property int stopMon: 0

    property alias e_startTime: startTime.text
    property alias e_stopTime: stopTime.text

    property alias e_deleteAllLogBtn_Down: deleteAllLogBtn.down


    width: 1020
    height: 806
    contentWidth:1000
    contentHeight:987
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    clip: true

    Image{  //日志区
        x:0
        y:0
        source: "icon/image2/logback.jpg"
        MyButton{//读取日志信息
            x:25
            y:92
            e_btnBackColor: "#00B5AC"
            e_text: qsTr("读取日志信息")
            e_btnBackW: 130
            e_btnBackH: 30
            onClicked: {
                if(isSerialOpen){
                    //读取日志数量,并清空已读的以及显示的日志信息
                    //logInfoModel.clear();
                    pageCnt=0;
                    logCnt=0;
                    showFlag=0;
                    bms.getLogCnt();

                }
                else{
                    msg.text="未打开串口！"
                    msg.open();
                }
            }
        }
        MyButton{//清除显示
            x:175
            y:92
            e_btnBackColor: "#040404"
            e_text: qsTr("清除显示")
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
                pageCnt=0;
                logCnt=0;
                logInfoModel.clearShow();

            }
        }

        Text{
            id:timeSelect
            x:337
            y:92
            font.family: "微软雅黑"
            font.pixelSize: 18
            text:qsTr("时间筛选：")
            color:"#333333"
        }
        Rectangle{
            id:rec
            x:427
            anchors.verticalCenter: timeSelect.verticalCenter
            width:381
            height: 30
            color:"#F2F7FE"
            border.color: "#C3C8D0"
            border.width: 1
            radius: 5
            Row{

                x:10
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10

                Image{
                    x:10
                    anchors.verticalCenter: parent.verticalCenter
                    source: "icon/image2/iconcalendar.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            selectTime.e_flag=0;
                            //得到年、月、日
                            // selectTime.e_yearModel.clear();

                            //                            startYear=logInfoModel.getStartYear();
                            //                            stopYear=logInfoModel.getStopYear();
                            //                            for(var i=startYear;i<=stopYear;i++){
                            //                                selectTime.e_yearModel.append({"modelData":i});
                            //                            }
                            //                            if(selectTime.e_yearModel.count==1){
                            //                                selectTime.e_yearModel.append({"modelData":0});
                            //                            }

                            selectTime.show()
                        }
                    }
                }
                TextInput {
                    id: startTime
                    text: qsTr("0000-00-00 00:00:00")
                    width: 136
                    font.family: "微软雅黑"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                Text {
                    text: qsTr("To")
                    font.family: "微软雅黑"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                Image{
                    anchors.verticalCenter: parent.verticalCenter
                    source: "icon/image2/iconcalendar.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            selectTime.e_flag=1;
                            //得到年、月、日
                            // selectTime.e_yearModel.clear();

                            //                            startYear=logInfoModel.getStartYear();
                            //                            stopYear=logInfoModel.getStopYear();
                            //                            for(var i=startYear;i<=stopYear;i++){
                            //                                selectTime.e_yearModel.append({"modelData":i});
                            //                            }
                            //                            if(selectTime.e_yearModel.count==1){
                            //                                selectTime.e_yearModel.append({"modelData":0});
                            //                            }

                            selectTime.show()
                        }
                    }

                }
                TextInput {
                    id: stopTime
                    text: qsTr("0000-00-00 00:00:00")
                    width: 136
                    font.family: "微软雅黑"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        MyButton{
            anchors.left: rec.right
            anchors.leftMargin: 15
            anchors.verticalCenter: timeSelect.verticalCenter
            e_btnBackColor:down?"gray":"#00B5AC"
            e_text: down?qsTr("返回"):qsTr("筛选")
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
                if(down){
                    //返回，显示全部
                    down=false;
                    pageCnt=0;
                    logCnt=0;
                    showFlag=0;
                    logInfoModel.getRxLog();
                    indicator.currentIndex=0;
                }
                else{
                    //筛选
                    down=true;
                    console.log("----------按下筛选按钮--------------")
                    pageCnt=0;
                    logCnt=0;
                    showFlag=1;
                    logInfoModel.getNeedLog(startTime.text,stopTime.text);
                    indicator.currentIndex=0;
                }


            }
        }


//        ParamConfigInput{
//            id:rtcTime
//            x:25
//            y:43
//            e_text: qsTr("当前RTC时间：")
//            e_recX: 116
//            e_recWidth: 164
//            e_recHeight: 30

//        }

//        MyButton{//读取rtc时间
//            x:338
//            anchors.verticalCenter: rtcTime.verticalCenter
//            e_btnBackColor: "#00B5AC"
//            e_text: qsTr("读取")
//            e_btnBackW: 100
//            e_btnBackH: 30
//            onClicked: {
//                if(isSerialOpen){
//                    bms.getRtcTime();
//                }
//                else{
//                    msg.text="未打开串口！"
//                    msg.open();
//                }
//            }
//        }


        TableView{
            id:logTable
            anchors.horizontalCenter: parent.horizontalCenter
            y:151
            width: 926
            height:451
            backgroundVisible: false
            frameVisible: false
            focus: false
            horizontalScrollBarPolicy:Qt.ScrollBarAlwaysOff
            verticalScrollBarPolicy : Qt.ScrollBarAlwaysOff
            clip:true
            headerDelegate:Rectangle{
                anchors.centerIn: parent
                height: 41
                Text{
                    anchors.centerIn: parent
                    text:styleData.value
                    font.family: "微软雅黑"
                    font.pixelSize: 16
                    color:"#333333"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            itemDelegate:Loader{
                id:loader
                anchors.centerIn: parent
                property var getValue
                property var colNo: styleData.column
                property var rowNo:styleData.row
                property int pageNo: indicator.currentIndex
                property string viewName: "删除日志"
                sourceComponent: {
                    if(styleData.column===0)
                    {
                        getValue=styleData.row+1+indicator.currentIndex*10;
                    }
                    else{
                        if(styleData.column===3 || styleData.column===4){
                            getValue=digit(styleData.value);
                        }
                        else
                            getValue=styleData.value;

                    }
                    return dispText
                }
            }
            onRowCountChanged:{
                console.log("rowCount",rowCount);
                deleteSelectModel.clear();
                for(var i=0;i<rowCount;i++){
                    deleteSelectModel.append({"value":logInfoModel.getDeleteLog(i,indicator.currentIndex,showFlag)})
                }
            }

            rowDelegate: Rectangle{
                width:944
                height:41
                color:"white"
            }

            TableViewColumn {
                role: "deleteRole"
                title:qsTr("序号")
                width: 58
            }
            TableViewColumn {
                role: "eventTypeRole"
                title: qsTr("事件类型")
                width: 91
            }

            TableViewColumn {
                role: "dateRole"
                title:qsTr("时间")
                width: 163
            }
            TableViewColumn {
                role: "totalVRole"
                title: qsTr("总电压V")
                width: 85
            }
            TableViewColumn {
                role: "totalIRole"
                title: qsTr("总电流A")
                width: 85
            }
            TableViewColumn {
                role: "tempRole"
                title: qsTr("最高温度℃")
                width: 104
            }
            TableViewColumn {
                role: "capRole"
                title: qsTr("满充容量mAh")
                width: 113
            }
            TableViewColumn {
                role: "leftCapRole"
                title: qsTr("剩余容量mAh")
                width: 114
            }
            TableViewColumn {
                role: "bmsStatusRole"
                title: qsTr("电池状态H")
                width: 113
            }
            model: logInfoModel
            Component.onCompleted: {
                bms.setLogInfoModel(logInfoModel)
            }
        }
        ListView{
            id:deleteSelectList
            anchors.left: logTable.left
            anchors.leftMargin: -10
            y:191
            width:20
            height:410
            model:deleteSelectModel
            delegate: Rectangle{
                width:20
                height:41
                Image{
                    id:img

                    source:value?"/icon/image2/select.png": "/icon/image2/noselect.png";
                    anchors.centerIn: parent

                    MouseArea{
                        anchors.fill: parent


                        onClicked: {
                            if(value){
                                console.log("取消",index,indicator.currentIndex)
                                logInfoModel.recordDeleteLog(index,indicator.currentIndex,false,showFlag);
                                img.source= "/icon/image2/noselect.png";
                                value=false;
                            }
                            else{
                                //img.source= "/icon/image2/select.png";
                                console.log("按下",index,indicator.currentIndex)
                                logInfoModel.recordDeleteLog(index,indicator.currentIndex,true,showFlag);
                                img.source= "/icon/image2/select.png";
                                value=true;
                            }
                        }
                    }
                }


            }

        }


        ListModel{
            id:deleteSelectModel
        }

        Canvas{
            x:37
            y:151
            width:926
            height:451
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<11;i++)
                    ctx.fillRect(0,40+i*41, 926, 1);
            }
        }
        Component{
            id:pageText
            Rectangle{
                width: 30
                height:30
                Text{
                    font.family: "微软雅黑"
                    font.pixelSize: 16
                    text:pageNo
                    color:((indicator.currentIndex+1)==pageNo)?"#00B5AC":"black"
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:Text.AlignVCenter

                }
                MouseArea{
                    anchors.fill:parent
                    onClicked: {
                        console.log(pageNo-1)
                        logInfoModel.getPage(pageNo-1,showFlag);
                        indicator.currentIndex=(pageNo-1);
                    }
                }
            }
        }
        Component{
            id:pageOmit
            Rectangle{
                width: 30
                height:30
                Text{
                    font.family: "微软雅黑"
                    font.pixelSize: 16
                    text:"…"
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:Text.AlignVCenter

                }
            }
        }
        Component{
            id:pageBlank
            Rectangle{
                width: 0
                height:0
            }
        }
        Button{
            id:nextPageBtn
            x:961
            y:634
            width:10
            height:16
            enabled:(indicator.currentIndex==(indicator.count-1))?false:true
            background:Image {
                source: (indicator.currentIndex==(indicator.count-1))?"icon/image2/nextpagedis":"icon/image2/nextpage"
            }
            onClicked: {
                indicator.currentIndex=indicator.currentIndex+1;
                logInfoModel.getPage(indicator.currentIndex,showFlag);

            }
        }

        PageIndicator {
            id: indicator
            anchors.verticalCenter: nextPageBtn.verticalCenter
            anchors.right: nextPageBtn.left
            anchors.rightMargin: 10
            count:pageCnt
            currentIndex: 0
            delegate:Loader{
                property int pageNo:(index+1);
                sourceComponent: {
                    if(indicator.currentIndex<3 || (indicator.currentIndex+1)==indicator.count){
                        if(index<=3 || (index+1)==indicator.count){
                            return pageText
                        }
                        else if(index==4){
                            return pageOmit
                        }
                        else
                            return pageBlank;
                    }
                    else if((indicator.count-1-indicator.currentIndex)<=2){
                        var l=indicator.count-1-indicator.currentIndex;
                        if(index<(4-l) || index>=indicator.currentIndex)
                            return pageText
                        else if(index==(4-l))
                            return pageOmit
                        else
                            return pageBlank
                    }

                    else {
                        if(index==0||index==indicator.currentIndex||index==(indicator.currentIndex+1) || (index+1)==indicator.count){
                            return pageText
                        }
                        else if(index==(indicator.currentIndex-1)||index==(indicator.currentIndex+2)){
                            return pageOmit
                        }
                        else
                            return pageBlank;
                    }
                }
            }
        }
        Button{
            id:previousPageBtn
            anchors.verticalCenter: nextPageBtn.verticalCenter
            anchors.right: indicator.left
            anchors.rightMargin: 10
            width:10
            height:16
            enabled: (indicator.currentIndex==0)?false:true
            background:Image {
                source: (indicator.currentIndex==0)?"icon/image2/previouspagedis":"icon/image2/previouspage"
            }
            onClicked: {
                indicator.currentIndex=indicator.currentIndex-1;
                logInfoModel.getPage(indicator.currentIndex,showFlag);

            }
        }

        Button{
            id:deleteAllLogBtn
            x:27
            y:634
            onClicked:{
                if(down)
                    down=false;
                else
                    down=true;

            }

            background: Image{
                source:deleteAllLogBtn.down? "icon/image2/select.png":"icon/image2/noselect.png"
            }
        }
        Button{   //删除选中的按钮
            x:60
            width:100
            height:30
            anchors.verticalCenter: deleteAllLogBtn.verticalCenter
            background: Rectangle{
                width:100
                height:30
                color:"#F84712"
                Text{
                    anchors.centerIn: parent
                    font.family: "微软雅黑"
                    font.pixelSize: 16
                    color:"white"
                    text:qsTr("删除选中")
                }
            }
            onClicked: {
                tipsWin.e_pageNo=indicator.currentIndex
                tipsWin.e_showFlag=showFlag
                tipsWin.e_deleteAll=deleteAllLogBtn.down
                tipsWin.show();
            }
        }
        Image{
            x:174
            anchors.verticalCenter: deleteAllLogBtn.verticalCenter
            source: "icon/image2/object.png"
            visible: deleteAllLogBtn.down?true:false
        }
        Text{
            x:197
            anchors.verticalCenter: deleteAllLogBtn.verticalCenter
            text:qsTr("清空所有")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#F84712"
            visible: deleteAllLogBtn.down?true:false
        }
        Button{   //删除选中的按钮
            x:271
            width:120
            height:30
            anchors.verticalCenter: deleteAllLogBtn.verticalCenter
            background: Rectangle{
                width:120
                height:30
                color:"#F84712"
                Text{
                    anchors.centerIn: parent
                    font.family: "微软雅黑"
                    font.pixelSize: 16
                    color:"white"
                    text:qsTr("删除ROM日志")
                }
            }
            onClicked: {
                tips.text=qsTr("将清空下位机上的所有日志，确定清空？")
                tips.open();
            }
        }
        MessageDialog{
            id:tips
            standardButtons:StandardButton.Cancel|StandardButton.Ok
            onAccepted: {
                bms.deleteRomLog();
            }
            onRejected: {
                tips.close();
            }
        }

        Text{
            x:435
            anchors.verticalCenter: deleteAllLogBtn.verticalCenter
            text:qsTr("本页")+logTable.rowCount+qsTr("条，共")+logCnt+qsTr("条")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#999999"
        }
    }

    Image{ //历史保护计数及最大值背景
        x:0
        y:700
        source: "icon/image2/historyback.jpg"
        Text{
            id:title
            x:25
            y:29
            text:qsTr("历史保护计数及最大值")
            font.family: "微软雅黑"
            font.pixelSize: 16
        }
        MyButton{
            x:220
            anchors.verticalCenter: title.verticalCenter
            e_btnBackColor: "#1AB2A1"
            e_text: qsTr("得到统计数值")
            e_btnBackW: 150
            e_btnBackH: 36
            onClicked: {
                if(isSerialOpen){
                    bms.getNo101_115();
                }
                else{
                    msg.text=qsTr("串口未打开！")
                    msg.open();
                }
            }
        }

        ListView{
            id:historyList
            x:5
            y:62
            width:989
            height:205
            model:historyModel

            delegate: Control{
                x:0
                y:0
                width:989
                height: 41
                onHoveredChanged: {
                    if(hovered)
                        historyRec.color="#D6EAFE"
                    else
                        historyRec.color="white"
                }

                Rectangle{
                    anchors.fill: parent
                    id:historyRec

                    ParamConfigInput{
                        x:38
                        e_recX:137;
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_text: name
                        e_unit: ""
                        e_input: ""+value

                    }
                    ParamConfigInput{
                        x:377
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_recX: 137
                        e_text: name2
                        e_input: ""+value2
                        e_unit: {
                            if(index<4)
                                return "";
                            else
                                return "A";
                        }
                    }
                    ParamConfigInput{
                        x:693
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_recX: 137
                        e_text: name3
                        e_input: ""+value3
                        e_unit: {
                            if(index<4)
                                return "℃";
                            else
                                return "A";
                        }
                    }

                }
            }

        }
        Canvas{
            x:0
            y:0
            width:999
            height:287
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<5;i++)
                    ctx.fillRect(27,102+i*41, 944, 1);
                ctx.fillRect(341,62, 1,204);
                ctx.fillRect(656,62, 1,204);
            }
        }
        ListModel{
            id:historyModel
        }
    }


    property var nameInHistoryModel: ["短路次数","充电过流次数","放电过流次数","单体过压次数","单体欠压次数"]
    property var name2InHistoryModel: ["高温充电次数","低温充电次数","高温放电次数","低温放电次数","最大充电电流"]
    property var name3InHistoryModel: ["电芯充电最高温","电芯充电最低温","电芯放电最高温","电芯放电最低温","最大放电电流"]

    Component.onCompleted: {
        console.log("坐标",logTable.x,logTable.y)
        for(var i=0;i<5;i++){
            historyModel.append({name:nameInHistoryModel[i],name2:name2InHistoryModel[i],name3:name3InHistoryModel[i],
                                    value:0,value2:0,value3:0})
        }


    }
    function prefixInteger(num, length) {
        return ( "0000" + num ).substr( -length );
    }

    Connections{
        target: bms
        onReadLogOnePage:{
            pageCnt++;
            console.log("bms pageCnt++",pageCnt)
        }
        //showLogCnt(cnt)
        onShowLogCnt:{
            logCnt=cnt;
        }
    }
    Connections{
        target: logInfoModel
        onReadLogOnePage:{
            pageCnt++;
            console.log("logInfoModel pageCnt++",pageCnt)
        }
        onShowLogCnt:{
            logCnt=cnt;
        }
        onReadLogTotalPage:{
            pageCnt=cnt;
            console.log("logInfoModel pageCnt++",pageCnt)
        }

    }

    function digit(i){
        return parseFloat(i).toFixed(2);
    }

    function btnList(){
        for(var i=0;i<logTable.currentRow;i++){
            deleteSelectModel.append({value:false});
        }
    }
}

