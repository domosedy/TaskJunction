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
        x: root.width - 260
        y: style.headerHeight
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        focus: true

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: content

            border.color: style.primaryColor
            border.width: style.defaultBorderSize
            color: style.listBackgroundColor
            radius: style.defaultRadius
            Drag.active: true

            TextField {
                id: listName

                z: 1
                placeholderText: "New list"
                placeholderTextColor: "#bfbfbf"
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                font.pointSize: 16
                font.family: "Poppins"
                font.bold: true

                background: Rectangle {
                    implicitWidth: content.width - 20
                    implicitHeight: 36
                    radius: style.defaultRadius
                    color: style.textFormColor
                    border.color: style.listBackgroundColor
                    border.width: 1
                }

            }

            Button {
                z: 1
                implicitWidth: parent.width - 20
                implicitHeight: 36
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 10
                onClicked: {
                    createListPopup.close();
                    mainClient.create_list(listName.text);
                    listName.text = "";
                }

                Text {
                    text: "Add list"
                    font.family: "Poppins"
                    font.pointSize: 16
                    anchors.centerIn: parent
                    color: "white"
                }

                background: Rectangle {
                    color: parent.down ? Qt.lighter(style.primaryColor, 1.2) : (parent.hovered ? Qt.lighter(style.primaryColor, 1.2) : style.primaryColor)
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
            id: toMenuButton

            width: style.headerHeight
            height: style.headerHeight
            anchors.left: parent.left
            onClicked: {
                loader.source = "BoardSelect.qml";
            }
            icon.source: "back.svg"
            icon.width: width
            icon.height: height

            background: Rectangle {
                color: parent.down ? Qt.darker(style.headerBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.headerBackgroundColor, 1.2) : style.headerBackgroundColor)
            }

        }

        Text {
            text: mainClient ? mainClient.current_board_name : ""
            font.family: "Poppins"
            font.pointSize: 20
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: toMenuButton.right
            anchors.leftMargin: 10
        }

        Button {
            id: newListBtn

            width: 110
            height: parent.height / 2
            onClicked: {
                createListPopup.open();
            }
            anchors.right: parent.right
            anchors.top: copyBtn.bottom

            Text {
                text: "New list"
                font.family: "Poppins"
                font.pointSize: 16
                color: "white"
                anchors.centerIn: parent
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.headerBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.headerBackgroundColor, 1.2) : style.headerBackgroundColor)
            }

        }

        Button {
            id: copyBtn

            width: 110
            height: parent.height / 2
            onClicked: {
                mainClient.copy_board(0);
            }
            anchors.right: parent.right
            anchors.top: parent.top

            Text {
                text: mainClient ? (mainClient.current_board.is_remote ? "Copy" : "Upload") : ""
                font.family: "Poppins"
                font.pointSize: 16
                color: "white"
                anchors.centerIn: parent
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.headerBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.headerBackgroundColor, 1.2) : style.headerBackgroundColor)
            }

        }

        TextField {
            id: filterHolder

            z: 1
            placeholderText: ""
            placeholderTextColor: "#bfbfbf"
            anchors.right: newListBtn.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 5
            font.pointSize: 16
            font.family: "Poppins"
            font.bold: true
            clip: true
            onAccepted: {
                mainClient.set_filter(filterHolder.text, filterTypeSwitch.checked);
                for (let i = 0; i < listview.model.count; ++i) {
                    listview.itemAtIndex(i).update_filter(filterHolder.text);
                }
            }

            background: Rectangle {
                implicitWidth: 120
                implicitHeight: 36
                radius: style.defaultRadius
                color: style.textFormColor
                border.color: style.listBackgroundColor
                border.width: 1
            }

        }

        Switch {
            id: filterTypeSwitch

            z: 1
            implicitWidth: 76
            implicitHeight: 26
            anchors.right: filterHolder.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 10

            indicator: Rectangle {
                implicitWidth: filterTypeSwitch.implicitWidth
                implicitHeight: filterTypeSwitch.implicitHeight
                x: filterTypeSwitch.leftPadding
                y: parent.height / 2 - height / 2
                radius: 13
                color: style.textFormColor
                border.color: style.textFormColor

                Rectangle {
                    x: filterTypeSwitch.checked ? parent.width - width : 0
                    width: 26
                    height: 26
                    radius: 13
                    color: "white"
                    border.color: "white"
                }

            }

            contentItem: Text {
                text: filterTypeSwitch.checked ? "All" : "Any"
                font.family: "Poppins"
                font.pointSize: 12
                verticalAlignment: Text.AlignVCenter
                //horizontalAlignment: Text.AlignHCenter
                leftPadding: 28
                //Padding: 5
                color: "white"
            }

        }

    }

    Rectangle {
        id: boardMain

        width: root.width
        height: root.height
        color: style.primaryColor
        anchors.top: boardMenuBar.bottom
        anchors.topMargin: 30

        Item {
            id: cardDragContainer

            anchors.fill: parent
            z: 3
        }

        ListView {
            id: listview

            anchors.fill: parent
            anchors.leftMargin: 10
            model: mainClient ? mainClient.current_board : undefined
            orientation: ListView.Horizontal
            spacing: 30

            delegate: List {
            }

        }

    }

}
