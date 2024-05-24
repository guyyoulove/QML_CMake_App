import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick 2.12
Window{
    id:loginWin
    x:bmsApp.x+(bmsApp.width-width)/2
    y:bmsApp.y+(bmsApp.height-height)/2
    width: 600
    height: 300
    Column{
        spacing: 20
        x:50
        y:50
        TextEdit{
            id:idText
            font.family: "微软雅黑"
            font.pixelSize: 15
            text: login.createID();
            readOnly: true
            selectByMouse: true
            mouseSelectionMode:TextEdit.SelectWords
        }
        ParamConfigInput{
            id:license
            e_text:qsTr("授权码")
            e_recX:60
            e_recWidth: 300
            e_recHeight: 30
        }
        Button{
            text: "授权"
            onClicked: {
                login.check(idText.text,license.e_input);
            }
        }
    }

    Connections{
        target: login
        onMsg:{
            if("验证成功"==s){
                isValid=true;
            }
            else{
                isValid=false;
            }
            msg.text=s;
            msg.open();
        }
    }
}
