import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client
import "qrc:"

Rectangle {
    id: auth
    width: root.width
    height: root.height
    color: style.primaryColor

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
            Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 20
                Text {
                    id: test_text
                    anchors.centerIn: parent
                    text: "Log in"
                    color: "black"
                    font.pointSize: 24
                    font.family: "Courier"
                }
                Layout.preferredWidth: 200
                Layout.preferredHeight: 40
                onClicked: {
                    var state = mainClient.login(loginUsername.text, loginPassword.text, loginIP.text, loginPort.text)
                    console.log(Client.success)
                    console.log(state)
                    if (state == 0) {
                        test_text.text = "CONN"
                    } else {
                        test_text.text = "FUU"
                    }
                }
            }                                          
        }
    }
}