import QtQuick 2.0

Text{
    property alias inputText: input.text
    //property alias cpenable: input.enabled
    height:30
    width:200
    font.family: "微软雅黑"
    font.pixelSize: 15
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    Rectangle{
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.right
        anchors.leftMargin: 10
        border.color: "#C8C8C8"
        width:100
        height:30
        TextInput{
            id:input
            font.family: "微软雅黑"
            font.pixelSize: 15
            anchors.fill: parent
            horizontalAlignment: TextInput.AlignHCenter
            verticalAlignment: TextInput.AlignVCenter

        }
    }
}
