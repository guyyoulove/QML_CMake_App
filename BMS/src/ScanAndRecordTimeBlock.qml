import QtQuick 2.7
import QtQuick.Controls 2.1


Text{  //扫描时间设定和记录时间间隔
    font.family: "微软雅黑"
    font.pixelSize: 18
    color:"#999999"
    property alias e_value: spin.value
    SpinBox{
        id:spin
        anchors.left: parent.right
        anchors.leftMargin: 14
        anchors.verticalCenter: parent.verticalCenter
        from:500
        to:10000
        value: 1000
        stepSize: 500
        font.family: "微软雅黑"
        font.pixelSize: 16
        width: 127
        height: 30
        editable: true
    }
    Text{
        id:unit
        anchors.left: spin.right
        anchors.leftMargin: 5
        font.family: "微软雅黑"
        font.pixelSize: 14
        anchors.verticalCenter: parent.verticalCenter
        verticalAlignment:TextInput.AlignVCenter
        color:"#666666"
        text: "mS"
    }


    MyButton{
        x:296
        anchors.verticalCenter: parent.verticalCenter
        e_btnBackW:75
        e_btnBackH: 35
        e_text: qsTr("SET")
        e_btnBackColor: "#00B5AC"
        onClicked: {
            if("扫描时间设定"==parent.text){
                e_x4.e_input=spin.value;
                bms.changeDelay(x1.e_input,x2.e_input,x3.e_input,x4.e_input);
            }

            else if("记录时间间隔"==parent.text){
                //Q_INVOKABLE void record_Rx_Scale(quint32 scanTime,quint32 recordTime);
                bms.record_Rx_Scale(scanTime.e_value,spin.value);
            }


        }

    }
}
