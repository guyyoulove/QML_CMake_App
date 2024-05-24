import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.5
import QtQuick 2.7
ScrollView { //参数配置页面
    width: 1020
    height: 806
    contentWidth:1000
    contentHeight:1250
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    clip: true

    property alias e_capModel: capModel
    property alias e_basicModel: basicModel
    property alias e_advancedModel: advancedModel
    property alias e_otherModel: otherModel
    property alias e_chargeModel: chargeModel
    property alias e_resistor: resistor.e_input
    property alias e_fdgl2_1_content: fdgl2_1.e_content
    property alias e_fdgl2_1_delay_content: fdgl2_1_delay.e_content
    property alias e_short_1_content: short_1.e_content
    property alias e_short_1_delay_content: short_1_delay.e_content



    x:0
    Image{
        y:0
        source: "/icon/image2/operateback.jpg"
        Text{
            x:25
            y:33
            text:qsTr("操作")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }
        Column{  //操作按钮
            x:25
            y:66
            spacing: 20
            Row{
                spacing: 20
                Button{
                    id:writeBtn
                    background: Rectangle{
                        implicitWidth:147
                        implicitHeight:48
                        color:writeBtn.pressed?"gray":"#1AB2A1"
                        radius: 10
                        Image{
                            x:24
                            y:12
                            source:"icon/image2/iconwrite.png"
                            Text{
                                x:34
                                anchors.verticalCenter: parent.verticalCenter
                                text:qsTr("写入参数")
                                font.family: "微软雅黑"
                                font.pixelSize: 16
                                color:"#FFFFFF"
                            }
                        }
                    }
                    onClicked: {
                        if(isSerialOpen){
                            bms.paramConfig(2);
                        }
                        else
                        {
                            msg.text=qsTr("未打开串口！")
                            msg.open();
                        }
                    }
                }
                Button{
                    id:importBtn
                    background: Rectangle{
                        implicitWidth:147
                        implicitHeight:48
                        color:importBtn.pressed?"gray":"#1AB2A1"
                        radius: 10
                        Image{
                            x:24
                            y:12
                            source:"icon/image2/iconimport.png"
                            Text{
                                x:34
                                anchors.verticalCenter: parent.verticalCenter
                                text:qsTr("导入参数")
                                font.family: "微软雅黑"
                                font.pixelSize: 16
                                color:"#FFFFFF"
                            }
                        }
                    }
                    onClicked: {
                        fileDialog.viewName="导入参数";
                        fileDialog.title=qsTr("导入参数：请选择一个excel文件")
                        fileDialog.nameFilters=["excel files (*xlsx)"];
                        fileDialog.open();

                    }
                }
            }
            Row{
                spacing: 20
                Button{
                    id:readBtn
                    background: Rectangle{
                        implicitWidth:147
                        implicitHeight:48
                        color:readBtn.pressed?"gray":"#1AB2A1"
                        radius: 10
                        Image{
                            x:24
                            y:12
                            source:"icon/image2/iconread.png"
                            Text{
                                x:34
                                anchors.verticalCenter: parent.verticalCenter
                                text:qsTr("读取参数")
                                font.family: "微软雅黑"
                                font.pixelSize: 16
                                color:"#FFFFFF"
                            }
                        }
                    }
                    onClicked: {
                        if(isSerialOpen){
                            bms.readParamConfig();
                        }
                        else{
                            msg.text=qsTr("未打开串口！")
                            msg.open();
                        }
                    }
                }
                Button{
                    id:exportBtn
                    background: Rectangle{
                        implicitWidth:147
                        implicitHeight:48
                        color:exportBtn.pressed?"gray":"#1AB2A1"
                        radius: 10
                        Image{
                            x:24
                            y:12
                            source:"icon/image2/iconexport.png"
                            Text{
                                x:34
                                anchors.verticalCenter: parent.verticalCenter
                                text:qsTr("导出参数")
                                font.family: "微软雅黑"
                                font.pixelSize: 16
                                color:"#FFFFFF"
                            }
                        }
                    }
                    onClicked: {
                        fileDialog.viewName="导出参数";
                        fileDialog.title=qsTr("导出参数：请选择一个excel文件")
                        fileDialog.nameFilters=["excel files (*xlsx)"];
                        fileDialog.open();
                    }
                }
            }
        }



    }



    Image{  //基本保护
        x:382
        y:0
        source: "/icon/image2/basicback.jpg"
        Text{
            x:26
            y:33
            text:qsTr("基本保护参数配置")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }

        ListView{

            id:basicList
            x:5
            y:66
            width:609
            height:410
            model:basicModel

            delegate: Control{
                x:0
                y:0
                width:609
                height: 41
                onHoveredChanged: {
                    if(hovered)
                        basicRec.color="#D6EAFE"
                    else
                        basicRec.color="white"
                }

                Rectangle{
                    anchors.fill: parent
                    id:basicRec

                    ParamConfigInput{
                        e_viewFlag:"基本保护"   //viewFlag  view的标识
                        e_col: 0
                        e_row: index
                        x:40
                        anchors.verticalCenter: parent.verticalCenter
                        e_text: name
                        e_input: ""+value
                        e_unit: {
                            if(index<2)
                                return "mV"
                            else if(index<4)
                                return "V"
                            else if(index<8)
                                return "℃"
                            else
                                return "A"
                        }
                    }
                    ParamConfigInput{
                        e_viewFlag:"基本保护"
                        e_col: 1
                        e_row: index
                        e_input: ""+value2
                        x:251
                        anchors.verticalCenter: parent.verticalCenter
                        e_text: qsTr("延时")
                        e_unit:  qsTr("S")
                    }
                    ParamConfigInput{
                        e_viewFlag:"基本保护"
                        e_col: 2
                        e_row: index
                        e_input: ""+value3
                        x:413
                        anchors.verticalCenter:parent.verticalCenter
                        e_text: {
                            if(index<4)
                                return qsTr("释放电压")
                            else if(index<8)
                                return qsTr("释放温度")
                            else
                                return qsTr("释放时间")
                        }
                        e_unit: {
                            if(index<2)
                                return "mV"
                            else if(index<4)
                                return "V"
                            else if(index<8)
                                return "℃"
                            else
                                return "S"

                        }
                    }
                }
            }

        }

        Canvas{
            x:0
            y:0
            width:617
            height:493
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<10;i++)
                    ctx.fillRect(26,106+i*41, 562, 1);
            }
        }
        ListModel{
            id:basicModel


        }
    }
    //-------------------------------------
    Image{ //容量参数配置
        x:0
        y:224
        source: "/icon/image2/capback.jpg"
        Text{
            x:25
            y:29
            text:qsTr("容量参数配置")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }

        ListView{
            id:capList
            x:4.5
            y:62
            width:356
            height:1654
            model:capModel

            delegate: Control{
                width:356
                height: 41
                onHoveredChanged: {
                    if(hovered)
                        capRec.color="#D6EAFE"
                    else
                        capRec.color="white"
                }
                Rectangle{
                    anchors.fill: parent
                    id:capRec
                    Text{
                        x:43
                        anchors.verticalCenter: parent.verticalCenter
                        text:name
                        font.family: "微软雅黑"
                        font.pixelSize: 16
                        color:"#333333"
                        Rectangle{
                            x:(index>2)?132:81
                            anchors.verticalCenter: parent.verticalCenter
                            width:70
                            height: 30
                            color:"#F2F7FE"
                            border.color: "#C3C8D0"
                            border.width: 1
                            radius: 5
                            TextInput{
                                property var realValue
                                text:value
                                anchors.fill: parent
                                horizontalAlignment: TextInput.AlignHCenter
                                verticalAlignment: TextInput.AlignVCenter
                                font.family: "微软雅黑"
                                font.pixelSize: 14
                                color:"#333333"
                                onTextChanged: {
                                    if(text!=""){
                                        if(index<2){
                                            realValue=text/10;
                                        }
                                        else if(2==index){
                                            realValue=text*100;
                                        }
                                        else
                                            realValue=text;
                                        bms.saveParam(realValue,index,0,"容量参数");
                                    }
                                    else{
                                        bms.saveParam(0,index,0,"容量参数");
                                    }

                                    // }
                                }
                            }
                            Text{
                                x:77
                                anchors.verticalCenter: parent.verticalCenter
                                font.family: "微软雅黑"
                                font.pixelSize: 14
                                color:"#666666"
                                text:unit
                            }
                        }
                    }
                }
            }
        }

        Canvas{
            x:0
            y:0
            width:365
            height:733
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<16;i++)
                    ctx.fillRect(27,102+i*41, 310, 1);
            }
        }
        ListModel{
            id:capModel


        }
    }
    //---------------------------------------
   Image{ //充电机参数配置
       x:0
       anchors.bottom:other.bottom
       anchors.bottomMargin: 2
       source: "icon/image2/paraback2.jpg"
       Text{
           x:26
           y:33
           text:qsTr("充电机参数配置")
           font.family: "微软雅黑"
           font.pixelSize: 16
           color:"#040404"
       }
       ListView{
           x:5
           y:66
           width:355
           height:280
           model:chargeModel

           delegate: Control{
               x:0
               y:0
               width:355
               height: 41
               onHoveredChanged: {
                   if(hovered)
                       chargeRec.color="#D6EAFE"
                   else
                       chargeRec.color="white"
               }

               Rectangle{
                   id:chargeRec
                   anchors.fill: parent
                   ParamConfigInput{
                       e_viewFlag: "充电机参数"
                       e_col: 0
                       e_row: index
                       x:38
                       e_text: name
                       e_unit: unit
                       e_recX: 153
                       e_recWidth: 70
                       e_input:value
                       anchors.verticalCenter: parent.verticalCenter
                   }


               }
           }

       }
       Canvas{
           x:0
           y:0
           width:365
           height:280
           onPaint: {
               var ctx = getContext("2d");
               ctx.fillStyle = "#EFF3F8"
               for(var i=0;i<3;i++)
                   ctx.fillRect(27,106+i*41, 310, 1);

           }
       }
       ListModel{
           id:chargeModel

       }
   }

    //---------------------------------
    Image{//高级参数配置
        x:382
        y:510
        source: "/icon/image2/advancedback.jpg"
        Text{
            x:26
            y:33
            text:qsTr("高级保护参数配置")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }

        ListView{

            x:5
            y:66
            width:609
            height:410
            model:advancedModel

            delegate: Control{
                x:0
                y:0
                width:609
                height: 41
                onHoveredChanged: {
                    if(hovered)
                        advancedRec.color="#D6EAFE"
                    else
                        advancedRec.color="white"
                }

                Rectangle{
                    id:advancedRec
                    anchors.fill: parent
                    Loader{
                        id:loader
                        x:38
                        width:609
                        height:41
                        property var getValue: value
                        property var getValue2: value2
                        property var getIndex: index
                        property var getContent
                        property var getName:name
                        sourceComponent:{
                            if(index<2){
                                return component1
                            }
                            else{
                                if(2==index){
                                    getContent=["预留","断开充电器","放电恢复"]
                                }
                                else if(3==index || 4==index ){
                                    getContent=["移除负载","充电解除"]
                                }
                                else if(5==index){
                                    getContent=["移除负载"]
                                }
                                return component2
                            }
                        }
                    }


                }
            }

        }




        Canvas{
            x:0
            y:0
            width:617
            height:330
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<6;i++)
                    ctx.fillRect(27,106+i*41, 562, 1);
            }
        }
        ParamConfigCombo{
            id:fdgl2_1
            property string viewFlag:"高级保护"
            e_row:0
            e_col: 2
            x:377
            y:76
            e_boxWidth:70
            e_boxBackWidth:70
            e_text: qsTr("挡位参考")
        }
        ParamConfigCombo{
            id:fdgl2_1_delay
            property string viewFlag:"高级保护"
            e_row:0
            e_col: 3
            x:452
            y:76
            e_boxWidth:70
            e_boxBackWidth:70
            e_text: qsTr("")

        }
        ParamConfigCombo{
            id:short_1
            property string viewFlag:"高级保护"
            e_row:1
            e_col: 2
            x:377
            y:117
            e_boxWidth:70
            e_boxBackWidth:70
            e_text: qsTr("挡位参考")
        }
        ParamConfigCombo{
            id:short_1_delay
            property string viewFlag:"高级保护"
            e_row:1
            e_col: 3
            x:452
            y:117
            e_boxWidth:70
            e_boxBackWidth:70
            e_text: qsTr("")

        }
        ListModel{
            id:advancedModel
        }

        ParamConfigInput{
            id:resistor
            e_viewFlag:"采样电阻值"
            e_col: 0
            e_row: 0
            property int index: 0
            x:416
            y:281
            e_text: qsTr("采样电阻值")
            e_unit: qsTr("mΩ")
            e_recX: 89
            e_input:"2"
        }

    }
    Image{ //其他参数
        id:other
        x:382
        y:857
        source: "/icon/image2/otherback.jpg"
        Text{
            x:26
            y:33
            text:qsTr("其他参数配置")
            font.family: "微软雅黑"
            font.pixelSize: 16
            color:"#040404"
        }
        ListView{

            x:5
            y:66
            width:609
            height:307
            model:otherModel

            delegate: Control{
                x:0
                y:0
                width:609
                height: (index==6)?61:41
                onHoveredChanged: {
                    if(hovered)
                        otherRec.color="#D6EAFE"
                    else
                        otherRec.color="white"
                }

                Rectangle{
                    id:otherRec
                    anchors.fill: parent
                    ParamConfigInput{
                        e_viewFlag: "其他参数"
                        e_col: 0
                        e_row: index
                        x:38
                        e_text: name
                        e_unit: unit
                        e_recX: 153
                        e_input: ""+value
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    ParamConfigInput{
                        e_viewFlag: "其他参数"
                        e_col: 1
                        e_row: index
                        x:320
                        e_text: name2
                        e_unit: unit2
                        e_input: ""+value2
                        e_recX: 153
                        anchors.verticalCenter: parent.verticalCenter
                    }

                }
            }

        }
        Canvas{
            x:0
            y:0
            width:617
            height:390
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<6;i++)
                    ctx.fillRect(27,106+i*41, 562, 1);
                ctx.fillRect(27,373, 562, 1);
                ctx.fillRect(308,66,1,307);
            }
        }
        ListModel{
            id:otherModel

        }

    }

    Component{
        id:component1
        ParamConfigInput{
            e_viewFlag:"高级保护"
            e_col: 0
            e_row: getIndex
            anchors.verticalCenter: parent.verticalCenter
            e_recX: 81
            e_text: getName
            e_input: ""+getValue
            e_unit: {
                if(getIndex<2)
                    return "A"
                else
                    return ""
            }

            ParamConfigInput{
                e_viewFlag:"高级保护"
                e_row: getIndex
                e_col: 1
                e_input: ""+getValue2
                x:182
                anchors.verticalCenter: parent.verticalCenter
                e_recX: 40
                e_text: qsTr("延时")
                e_unit:  {
                    if(getIndex==0)
                        return "mS"
                    else if(getIndex==1)
                        return "uS"
                    else
                        return ""
                }
            }
        }


    }
    Component{
        id:component2
        ParamConfigCombo{
            property string viewFlag:"高级保护"
            e_row:getIndex
            e_col:0
            e_content: getContent
            e_text: getName
            e_index:getValue
            /*{
                if(2==getRow)
                    return 1
                else
                    return 0
            }*/

            e_boxX: 184
            anchors.verticalCenter: parent.verticalCenter
        }

    }

    property var nameInCapModel: ["设计容量","满充容量","自放电率","充电终止电压","充电终止电流",
        "放电终止电压","10%容量电压","20%容量电压","30%容量电压","40%容量电压"
        ,"50%容量电压","60%容量电压","70%容量电压","80%容量电压","90%容量电压","100%容量电压"]

    property var valueInCapModel: [2000,0,0.01,4200,100,3000,0,0,0,0,0,0,0,0,0,0]

    property var unitInCapModel: ["mAh","mAh","%","mV","mA","mV","mV","mV","mV","mV","mV","mV","mV","mV","mV","mV"]

    property var nameInBasicModel: ["单体过压","单体欠压","整组过压","整组欠压","充电高温","充电低温"
        ,"放电高温","放电低温","充电过流1","放电过流1"]
    property var name3InBasicModel: ["释放电压","释放电压","释放电压","释放电压","释放温度","释放温度"
        ,"释放温度","释放温度","释放时间","释放时间"]

    property var valueInBasicModel: [4200,3000,54.6,36.4,45,-5,60,-20,15.0,45]
    property var value2InBasicModel: [2,5,2,5,0,0,0,0,2,4]
    property var value3InBasicModel: [4100,3100,53.3,37.7,42,-3,55,-15,0,0]



    property var nameInAdvanced: ["放电过流2","短路保护","充电过流保护释放条件","放电过流1保护释放条件",
        "放电过流2保护释放条件","短路保护释放条件"]

    property var valueInAdvanced: [80,150,0,0,0,0]
    property var value2InAdvanced: [320,200]


    property var nameInOther: ["零电流窗口","充电状态电流门限","放电状态电流门限",
        "均衡开启电压","均衡开启压差","均衡停止压差","充电MOS\r\n强制拉开电流"]
    property var name2InOther: ["均衡开启延迟","PACK并机电压差","PACK并机电流",
        "PACK切机电压差","PACK切机电流","","放电MOS\r\n强制拉开电流"]

    property var valueInOther: [20,50,50,3900,60,20,-1000]
    property var value2InOther: [0,1000,1000,3000,1000,0,300]

    property var unitInOther: ["mA","mA","mA","mV","mV","mV","mA"]
    property var unit2InOther: ["S","mV","mA","mV","mA","","mA"]

    property var nameInCharge: ["最高允许充电电压","最高允许充电温度","最低允许充电温度"]
    property var unitInCharge: ["V","℃","℃"]
    property var valueInCharge: [0.01,0,0]

    Component.onCompleted: {
        for(var i=0;i<16;i++)
            capModel.append({name:nameInCapModel[i],unit:unitInCapModel[i],value:valueInCapModel[i]})

        for(i=0;i<10;i++){
            basicModel.append({name:nameInBasicModel[i],value:valueInBasicModel[i],value2:value2InBasicModel[i],value3:value3InBasicModel[i]})
        }
        //        for(i=0;i<4;i++){
        //            basicModel.append({name:nameInBasicModel[i+4],value:valueInBasicModel[i+4],value3:value3InBasicModel[i+4]})
        //        }

        //        basicModel.append({name:nameInBasicModel[8],value:valueInBasicModel[8],value2:value2InBasicModel[4]})
        //        basicModel.append({name:nameInBasicModel[9],value:valueInBasicModel[9],value2:value2InBasicModel[5]})

        for(i=0;i<2;i++){
            advancedModel.append({name:nameInAdvanced[i],value:valueInAdvanced[i],value2:value2InAdvanced[i]})
        }
        for(i=0;i<4;i++){
            advancedModel.append({name:nameInAdvanced[i+2],value:valueInAdvanced[i+2]})
        }
        for(i=0;i<7;i++){
            otherModel.append({name:nameInOther[i],unit:unitInOther[i],name2:name2InOther[i],unit2:unit2InOther[i],value:valueInOther[i],value2:value2InOther[i]})
        }
        for(i=0;i<3;i++){
            chargeModel.append({name:nameInCharge[i],unit:unitInCharge[i],value:valueInCharge[i].toString()})
        }
    }

}
