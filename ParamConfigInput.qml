import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.5
import QtQuick 2.7

Text{  //参数配置输入框
    property alias e_text: name.text
    property alias e_unit: unit.text
    property alias e_recX: rec.x
    property alias e_recWidth: rec.width
    property alias e_recHeight: rec.height
    property alias e_input: input.text
    property string e_viewFlag: ""
    property int e_col: 0
    property int e_row: 0
    id:name
    font.family: "微软雅黑"
    font.pixelSize: 16
    color:"#333333"
    verticalAlignment: Text.AlignVCenter
    Rectangle{
        id:rec
        x:(name.text=="延时")?47:80
        opacity: (name.text==""&&unit.text=="")?0:1
        anchors.verticalCenter: parent.verticalCenter
        width:56
        height: 30
        color:"#F2F7FE"
        border.color: "#C3C8D0"
        border.width: 1
        radius: 5
        TextInput{
            id:input
            property var realValue
            property string previousString: ""

            anchors.fill: parent
            horizontalAlignment: TextInput.AlignHCenter
            verticalAlignment: TextInput.AlignVCenter
            font.family: "微软雅黑"
            font.pixelSize: 14
            color:(input.text=="输入")?"#999999":"#333333"
            enabled: {
                if(e_viewFlag=="基本保护" && (index>=4 && index<=7) && e_col==1){
                    return false;
                }
                else if(e_viewFlag=="基本保护" && (index==9||index==8) && e_col==2){
                    return false;
                }
                else
                    return true;
            }
            onAccepted: {
                if("延时"==e_viewFlag){
                    focus=false;
                    bms.changeDelay(x1.e_input,x2.e_input,x3.e_input,x4.e_input);
                }
                else if("通讯失败次数"==e_viewFlag){
                    focus=false;
                    if(text!=""){
                        bms.setComFailCnt(text);
                    }
                    else{
                        bms.setComFailCnt(20);
                        text="20";
                    }
                }
            }

            onActiveFocusChanged: {
                if(activeFocus){
                    if(input.text=="输入"){
                        previousString="输入";
                        input.text="";
                    }
                }
                else{
                    if(input.text==""){ //什么都没有输入
                        if(previousString=="输入")
                            input.text="输入";
                    }
                }

            }


            onTextChanged: {


                if("基本保护"==e_viewFlag || "采样电阻值"==e_viewFlag || "其他参数"==e_viewFlag || "高级保护"==e_viewFlag || "充电机参数"==e_viewFlag){

                    if(text!=""){
                        if("基本保护"==e_viewFlag && (2==index||3==index) && (0==e_col||2==e_col)){
                            realValue=text*10;
                        }
                        else if("充电机参数"==e_viewFlag && 0==index){
                            realValue=text*100;
                        }
                        else if("基本保护"==e_viewFlag && 8==index && 0==e_col){
                            realValue=text*10;
                        }
                        else if("采样电阻值"==e_viewFlag){
                            realValue=text*10;
                        }
                        else if("其他参数"==e_viewFlag && 6==index && 0==e_col){
                            realValue=text*(-1);
                        }
                        else
                            realValue=text;
                        console.log("值",e_viewFlag,realValue);
                        bms.saveParam(realValue,e_row,e_col,e_viewFlag);
                    }
                    else{
                        bms.saveParam(0,e_row,e_col,e_viewFlag);
                    }

                }
                else if(e_viewFlag=="afe"){
                    if((e_row<2 && 0==e_col) ||(6==e_row && 2==e_col)){
                        //是十六进制的显示

                        bms.saveAfeHex(e_row,e_col,text);
                    }
                    else {
                        bms.saveAfe(e_row,e_col,text);
                    }
                }
            }
        }
        Text{
            id:unit
            anchors.left: parent.right
            anchors.leftMargin: 7
            anchors.verticalCenter: parent.verticalCenter
            font.family: "微软雅黑"
            font.pixelSize: 14
            color:"#666666"
        }
    }
}











