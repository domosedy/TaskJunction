import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client
import "qrc:"

Rectangle {
    id: boards_menu
    width: root.width
    height: root.height
    color: style.primaryColor

    ListView {
        id: boards_list
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.topMargin: 10
        model: mainClient.board_menu
        orientation: ListView.Vertical
        spacing: 5
        delegate: Rectangle {
            id: board_card;
            width: 300;
            height: 100;
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: name
                font.family: "Courier"
                font.pointSize: 16
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 5
            }
            Text {
                text: description
                font.family: "Courier"
                font.pointSize: 16
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 5
            }   
            MouseArea {
                anchors.fill: parent
                onDoubleClicked: {
                    mainClient.request_board(index);
                }
            }         
        }
    }
}
