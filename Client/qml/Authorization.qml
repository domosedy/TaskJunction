import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client
import "qrc:"

Rectangle {
    id: auth
    visible: true
    width: root.width
    height: root.height
    color: style.boardBackgroundColor

    Frame {
        id: login_frame
        anchors.centerIn: auth
        width: 320
        height: 320 
        background: Rectangle {
            color: style.boardBackgroundColor
        }        

        ColumnLayout {
            width: parent.width
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10

            TextField {
                id: loginUsername
                placeholderText: "Username"
                Layout.preferredWidth: parent.width - 20
                Layout.alignment: Qt.AlignHCenter
                color: "black"
                font.pointSize: 24
                font.family: "Courier"
                leftPadding: 30
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    radius: implicitHeight / 2
                    color: "white"
                }
            }
            TextField {
                id: loginPassword
                placeholderText: "Password"
                Layout.preferredWidth: parent.width - 20
                Layout.alignment: Qt.AlignHCenter
                color: "black"
                font.pointSize: 24
                font.family: "Courier"
                leftPadding: 30
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    radius: implicitHeight / 2
                    color: "white"
                }
                onTextChanged: background.color = "white"
            }
            TextField {
                id: loginIP
                placeholderText: "Server IP"
                Layout.preferredWidth: parent.width - 20
                Layout.alignment: Qt.AlignHCenter
                color: "black"
                font.pointSize: 24
                font.family: "Courier"
                leftPadding: 30
                text: "127.0.0.1"
                validator: RegularExpressionValidator {
                    regularExpression: /^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.?\b){4}$/
                }
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    radius: implicitHeight / 2
                    color: "white"
                }
                onTextChanged: background.color = "white"
            }
            TextField {
                id: loginPort
                placeholderText: "Server Port"
                Layout.preferredWidth: parent.width - 20
                Layout.alignment: Qt.AlignHCenter
                color: "black"
                font.pointSize: 24
                font.family: "Courier"
                leftPadding: 30
                text: "3030"
                validator: RegularExpressionValidator {
                    regularExpression: /^((6553[0-5])|(655[0-2][0-9])|(65[0-4][0-9]{2})|(6[0-4][0-9]{3})|([1-5][0-9]{4})|([0-5]{0,5})|([0-9]{1,4}))$/
                }
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    radius: implicitHeight / 2
                    color: "white"
                }
            }
            Rectangle {
                Layout.preferredHeight: 40 
                Layout.preferredWidth: parent.width - 20
                Layout.alignment: Qt.AlignHCenter 
                color: style.boardBackgroundColor
                Row {  
                    anchors.fill: parent
                    spacing: 10
                    Button {
                        width: 40
                        height: width
                        background: Rectangle {
                            color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                            (parent.hovered ? Qt.darker(style.primaryColor, 1.2) : style.primaryColor)
                        }                             
                        Text {
                            text: "<"
                            font.family: "Courier"
                            font.pointSize: 24
                            anchors.centerIn: parent
                            color: "white"
                        }   
                        onClicked: {
                            loader.active = false
                            start_menu.visible = true
                        }
                    }                     
                    Button {
                        width: parent.width-50
                        height: parent.height
                        background: Rectangle {
                            color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                            (parent.hovered ? Qt.darker(style.primaryColor, 1.2) : style.primaryColor)
                        }                             
                        Text {
                            id: test_text
                            anchors.centerIn: parent
                            text: "Log in"
                            color: "white"
                            font.pointSize: 24
                            font.family: "Courier"
                        }
                        onClicked: {
                            mainClient.login(loginUsername.text, loginPassword.text, loginIP.text, loginPort.text)
                        }
                    }                                             
                }
            }                                         
        }
    }
    Connections {
        target: mainClient
        function onConnectionStatusChanged() {
            if (mainClient.connection_status == Client.Authorized) {
                loader.source = "BoardSelect.qml"
            } 
            if (mainClient.connection_status == Client.Authentification_failed) {
                loginPassword.background.color = style.deleteBackgroundColor
            }            
            if (mainClient.connection_status == Client.Unable_to_connect) {
                loginIP.background.color = style.deleteBackgroundColor
            }
        }
    }
}
