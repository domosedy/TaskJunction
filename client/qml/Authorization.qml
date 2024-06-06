import Client
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:"

Rectangle {
    id: authMain

    visible: true
    width: root.width
    height: root.height
    color: style.primaryColor

    Rectangle {
        id: loginFrame

        anchors.centerIn: authMain
        width: 336
        height: 480
        color: style.primaryColor

        Text {
            id: loginHeader

            anchors.top: parent.top
            anchors.left: parent.left
            color: "white"
            font.pointSize: 24
            font.bold: true
            font.family: "Poppins"
            text: "Connect to a server"
        }

        Text {
            id: usernameHeader

            anchors.top: loginHeader.bottom
            anchors.left: parent.left
            anchors.topMargin: 10
            color: "white"
            font.pointSize: 14
            font.family: "Poppins"
            text: "Username"
        }

        TextField {
            id: loginUsername

            placeholderText: "Enter your username"
            placeholderTextColor: "#bfbfbf"
            anchors.top: usernameHeader.bottom
            anchors.left: parent.left
            anchors.topMargin: 5
            font.pointSize: 16
            font.family: "Poppins"
            color: "white"

            background: Rectangle {
                implicitWidth: 336
                implicitHeight: 42
                radius: style.defaultRadius
                color: style.textFormColor
                border.color: style.listBackgroundColor
                border.width: style.defaultBorderSize
            }

        }

        Text {
            id: passwordHeader

            anchors.top: loginUsername.bottom
            anchors.left: parent.left
            anchors.topMargin: 10
            color: "white"
            font.pointSize: 14
            font.family: "Poppins"
            text: "Password"
        }

        TextField {
            id: loginPassword

            placeholderText: "Enter your password"
            placeholderTextColor: "#bfbfbf"
            anchors.top: passwordHeader.bottom
            anchors.left: parent.left
            anchors.topMargin: 5
            font.pointSize: 16
            font.family: "Poppins"
            color: "white"

            background: Rectangle {
                implicitWidth: 336
                implicitHeight: 42
                radius: style.defaultRadius
                color: style.textFormColor
                border.color: style.listBackgroundColor
                border.width: 1
            }

        }

        Text {
            id: ipHeader

            anchors.top: loginPassword.bottom
            anchors.left: parent.left
            anchors.topMargin: 10
            color: "white"
            font.pointSize: 14
            font.family: "Poppins"
            text: "Host"
        }

        TextField {
            id: loginIP

            placeholderText: "Enter server IP address"
            placeholderTextColor: "#bfbfbf"
            anchors.top: ipHeader.bottom
            anchors.left: parent.left
            anchors.topMargin: 5
            font.pointSize: 16
            font.family: "Poppins"
            color: "white"
            text: "127.0.0.1"
            onTextChanged: {
                background.color = style.textFormColor;
                loginPort.background.color = style.textFormColor;
            }

            validator: RegularExpressionValidator {
                regularExpression: /^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.?\b){4}$/
            }

            background: Rectangle {
                implicitWidth: 336
                implicitHeight: 42
                radius: style.defaultRadius
                color: style.textFormColor
                border.color: style.listBackgroundColor
                border.width: 1
            }

        }

        Text {
            id: portHeader

            anchors.top: loginIP.bottom
            anchors.left: parent.left
            anchors.topMargin: 10
            color: "white"
            font.pointSize: 14
            font.family: "Poppins"
            text: "Port"
        }

        TextField {
            id: loginPort

            placeholderText: "Enter server port"
            placeholderTextColor: "#bfbfbf"
            anchors.top: portHeader.bottom
            anchors.left: parent.left
            anchors.topMargin: 5
            font.pointSize: 16
            font.family: "Poppins"
            text: "3030"
            color: "white"
            onTextChanged: {
                background.color = style.textFormColor;
                loginIP.background.color = style.textFormColor;
            }

            validator: RegularExpressionValidator {
                regularExpression: /^((6553[0-5])|(655[0-2][0-9])|(65[0-4][0-9]{2})|(6[0-4][0-9]{3})|([1-5][0-9]{4})|([0-5]{0,5})|([0-9]{1,4}))$/
            }

            background: Rectangle {
                implicitWidth: 336
                implicitHeight: 42
                radius: style.defaultRadius
                color: style.textFormColor
                border.color: style.listBackgroundColor
                border.width: 1
            }

        }

        Rectangle {
            height: 40
            width: parent.width
            color: style.primaryColor
            anchors.top: loginPort.bottom
            anchors.left: parent.left
            anchors.topMargin: 20

            Button {
                anchors.left: parent.left
                width: 34
                height: 40
                onClicked: {
                    loader.active = false;
                    startMenu.visible = true;
                }
                icon.source: "back.svg"
                icon.width: width
                icon.height: height

                background: Rectangle {
                    color: parent.down ? Qt.darker(style.boardBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.boardBackgroundColor, 1.2) : style.boardBackgroundColor)
                    radius: style.defaultRadius
                }

            }

            Button {
                width: 281
                height: 40
                anchors.right: parent.right
                onClicked: {
                    mainClient.login(loginUsername.text, loginPassword.text, loginIP.text, loginPort.text);
                }

                Text {
                    anchors.centerIn: parent
                    text: "Log in"
                    color: "white"
                    font.pointSize: 16
                    font.family: "Poppins"
                }

                background: Rectangle {
                    color: parent.down ? Qt.darker(style.boardBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.boardBackgroundColor, 1.2) : style.boardBackgroundColor)
                    radius: style.defaultRadius
                }

            }

        }

    }

    Connections {
        function onConnectionStatusChanged() {
            if (mainClient.connection_status == Client.Authorized)
                loader.source = "BoardSelect.qml";

            if (mainClient.connection_status == Client.Authentification_failed)
                loginPassword.background.color = style.deleteBackgroundColor;

            if (mainClient.connection_status == Client.Unable_to_connect) {
                loginIP.background.color = style.deleteBackgroundColor;
                loginPort.background.color = style.deleteBackgroundColor;
            }
        }

        target: mainClient
    }

}
