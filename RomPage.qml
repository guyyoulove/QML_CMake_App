import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
Item {  //Rom页
     property alias e_romModel: romModel
    Image{
        x:0
        y:0


        source: "icon/image2/paramback.jpg"
        Text{
            x:25
            y:29
            text:qsTr("AfeROM")
            font.family: "微软雅黑"
            font.pixelSize: 16
        }
        ListView{
            id:romList
            x:5
            y:62
            width:989
            height:369
            model:romModel

            delegate: Control{
                x:0
                y:0
                width:989
                height: 41
                onHoveredChanged: {
                    if(hovered)
                        romRec.color="#D6EAFE"
                    else
                        romRec.color="white"
                }

                Rectangle{
                    anchors.fill: parent
                    id:romRec

                    ParamConfigInput{
                        e_viewFlag:"afe"
                        x:38
                        e_recX:147;
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_text: name
                        e_unit: ""
                        e_input:value;


                        e_row: index
                        e_col: 0

                    }
                    ParamConfigInput{
                        e_viewFlag:"afe"
                        x:377
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_recX: 147
                        e_text: name2
                        e_unit: ""
                        e_input:value2
                        e_row:index
                        e_col: 1
                    }
                    ParamConfigInput{
                        e_viewFlag:"afe"
                        x:703
                        anchors.verticalCenter: parent.verticalCenter
                        e_recWidth: 70
                        e_recHeight: 30
                        e_recX: 137
                        e_text: name3
                        e_unit: ""
                        e_input:value3;

                        e_row:index
                        e_col: 2
                    }

                }
            }

        }
        Canvas{
            x:0
            y:0
            width:999
            height:451
            onPaint: {
                var ctx = getContext("2d");
                ctx.fillStyle = "#EFF3F8"
                for(var i=0;i<9;i++)
                    ctx.fillRect(27,102+i*41, 944, 1);
                ctx.fillRect(341,62, 1,368);
                ctx.fillRect(656,62, 1,368);
            }
        }
        ListModel{
            id:romModel
        }
    }

    Row{
        x:27
        y:517
        spacing: 89
        MyButton{
            e_text:qsTr("导入")
            e_btnBackColor: "#00B5AC"
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
                fileDialog.viewName="导入Afe";
                fileDialog.title=qsTr("导入Afe：请选择一个excel文件")
                fileDialog.nameFilters=["excel files (*xlsx)"];
                fileDialog.open();
            }
        }
        MyButton{
            e_text:qsTr("导出")
            e_btnBackColor: "#00B5AC"
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
                fileDialog.viewName="导出Afe";
                fileDialog.title=qsTr("导出Afe：请选择一个excel文件")
                fileDialog.nameFilters=["excel files (*xlsx)"];
                fileDialog.open();

            }
        }
        MyButton{
            e_text:qsTr("全部读取")
            e_btnBackColor: "#040404"
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
                bms.readAfe();
            }
        }
        MyButton{
            e_text:qsTr("全部写入")
            e_btnBackColor: "#F84712"
            e_btnBackW: 100
            e_btnBackH: 30
            onClicked: {
               bms.writeAfe();
            }
        }
    }

  MessageDialog{
      id:msgInAfe
      standardButtons : StandardButton.Cancel|StandardButton.Ok
      icon :StandardIcon.Warning
      title: qsTr("注意！")
      text:qsTr("请确保导出文件(‪D:/exportAfe.xlsx)已经关闭，否则会导出失败！")
      onAccepted: {
          bms.exportAfe(nameInRomModel,name2InRomModel,name3InRomModel);
      }

  }
    property var nameInRomModel: ["SCONF1M","SCONF2","OVT_LDRT_OVH","OVL","UVT-OVRH","OVRL","UV","UVR","BALV","PREV"]
    property var name2InRomModel: ["L0V","PFV","OCD1V_OCD1T","OCD2V_OCD2T","SCB_SCT","OCCV_OCCT","MOST_OCRT_PFT","OCT","OCTR","UTC"]
    property var name3InRomModel: ["UCTR","OTD","OTDR","UTD","UTDR","TRM","SCONF1S","TRS"]

    Component.onCompleted: {
        for(var i=0;i<10;i++){
            romModel.append({name:nameInRomModel[i],name2:name2InRomModel[i],name3:name3InRomModel[i],value:"",value2:"",value3:""})
        }
    }

}
