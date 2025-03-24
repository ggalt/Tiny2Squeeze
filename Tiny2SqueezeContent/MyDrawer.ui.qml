import QtQuick
import QtQuick.Controls
import QtCore

// import Tiny2Squeeze
Rectangle {
    id: myDrawer

    signal drawerClose

    property bool saveInfo: false
    property string serverIP: ""
    property string jsonPort: ""
    property string cliPort: ""
    property ListModel playerList: ListModel {}
    property alias playerName: comboBox.currentText
    property alias playerIndex: comboBox.currentIndex

    Label {
        id: comboBoxLabel
        color: "#222222"
        width: 100
        text: qsTr("Choose Player:")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 10
        anchors.topMargin: 10
    }

    ComboBox {
        id: comboBox
        anchors.left: comboBoxLabel.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 10
        font.pointSize: 12
        model: myDrawer.playerList
        textRole: "value"
    }

    Label {
        id: serverIPLabel
        width: 100
        color: "#222222"
        text: qsTr("Server IP:")
        anchors.left: parent.left
        anchors.top: comboBox.bottom
        anchors.leftMargin: 10
        anchors.topMargin: 10
    }

    Label {
        id: serverPortLabel
        width: 100
        color: "#222222"
        text: qsTr("Server Port:")
        anchors.left: parent.left
        anchors.top: serverIPLabel.bottom
        anchors.leftMargin: 10
        anchors.topMargin: 10
    }

    Label {
        id: cliPortLabel
        width: 100
        color: "#222222"
        text: qsTr("CLI Port:")
        anchors.left: parent.left
        anchors.top: serverPortLabel.bottom
        anchors.leftMargin: 10
        anchors.topMargin: 10
    }

    TextInput {
        id: editServerIP
        text: myDrawer.serverIP
        anchors.verticalCenter: serverIPLabel.verticalCenter
        anchors.left: serverIPLabel.right
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        font.pixelSize: 12
    }

    TextInput {
        id: editServerPort
        text: myDrawer.jsonPort
        anchors.verticalCenter: serverPortLabel.verticalCenter
        anchors.left: serverPortLabel.right
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        font.pixelSize: 12
    }

    TextInput {
        id: editCliPort
        text: myDrawer.cliPort
        anchors.verticalCenter: cliPortLabel.verticalCenter
        anchors.left: cliPortLabel.right
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        font.pixelSize: 12
    }

    Rectangle {
        id: controlsRect

        height: parent.height / 4

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.bottomMargin: 10

        ImageButton {
            id: okBtn
            height: controlsRect.height
            width: height
            anchors.right: controlsRect.horizontalCenter
            anchors.rightMargin: 20
            source: "images/check-mark-png-45005.png"

            Connections {
                target: okBtn
                function onImageClicked() {
                    myDrawer.saveInfo = true
                    myDrawer.serverIP = editServerIP.text
                    myDrawer.cliPort = editCliPort.text
                    myDrawer.jsonPort = editServerPort.text
                    // console.log("myDrawer.serverIP:", myDrawer.serverIP, "editServerIP.text", editServerIP.text)
                    myDrawer.drawerClose()
                }
            }
        }

        ImageButton {
            id: cancelBtn
            height: controlsRect.height
            width: height
            anchors.left: controlsRect.horizontalCenter
            anchors.leftMargin: 20
            source: "images/error-icon-25243.png"

            Connections {
                target: cancelBtn
                function onImageClicked() {
                    myDrawer.saveInfo = false
                    myDrawer.drawerClose()
                }
            }
        }
    }
}
