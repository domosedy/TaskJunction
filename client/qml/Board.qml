import Client
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:"

Rectangle {
    id: board

    visible: true

    Popup {
        id: createListPopup

        width: 210
        height: 120
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: content

            border.color: style.primaryColor
            border.width: style.defaultBorderSize
            Drag.active: true

            TextField {
                id: listName

                z: 1
                placeholderText: "New list"
                font.family: "Courier"
                font.pointSize: 12
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 10
            }

            Button {
                z: 1
                height: style.smallButtonSize
                width: height * 2.5
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                onClicked: {
                    createListPopup.close();
                    mainClient.create_list(listName.text);
                    listName.text = "";
                }

                Text {
                    text: "Add"
                    font.family: "Courier"
                    font.pointSize: 12
                    anchors.centerIn: parent
                }

                background: Rectangle {
                    color: parent.down ? Qt.lighter(style.createBackgroundColor, 1.4) : (parent.hovered ? Qt.lighter(style.createBackgroundColor, 1.2) : style.createBackgroundColor)
                }

            }

            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }

        }

    }

    Rectangle {
        id: boardMenuBar

        visible: true
        width: root.width
        height: style.headerHeight
        color: style.headerBackgroundColor
        anchors.top: root.top
        z: 1

        Button {
            width: style.smallButtonSize
            height: width
            onClicked: {
                loader.source = "BoardSelect.qml";
            }

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter


            Text {
                text: "<"
                font.family: "Courier"
                font.pointSize: 12
                anchors.centerIn: parent
                color: "white"
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.primaryColor, 1.4) : (parent.hovered ? Qt.darker(style.primaryColor, 1.2) : style.primaryColor)
            }

        }

        Button {
            width: style.smallButtonSize
            height: width
            onClicked: {
                createListPopup.open();
            }

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter       

            Text {
                anchors.centerIn: parent
                text: "+"
                color: "white"
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.primaryColor, 1.4) : (parent.hovered ? Qt.darker(style.primaryColor, 1.2) : style.primaryColor)
            }

        }

        Text {

            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter        

            text: mainClient.current_board_name
            y: 3
            height: style.smallButtonSize
            font.family: "Courier"
            font.pointSize: 16
            font.bold: true
            color: "white"
        }
    }

    Rectangle {
        id: boardMain

        width: root.width
        height: root.height
        color: style.boardBackgroundColor
        y: boardMenuBar.height+10

        ListView {
            id: listview
            anchors.fill: parent
            anchors.leftMargin: 10
            model: mainClient.current_board
            orientation: ListView.Horizontal
            spacing: 15

            delegate: List {
            }
            // Rectangle {
            //     anchors.fill: parent
            //     color: "red"
            // }
        }

    }

}
