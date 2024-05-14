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
        x: root.width / 2
        y: root.height / 2
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        focus: true

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: content

            border.color: style.listBackgroundColor
            border.width: style.defaultBorderSize
            color: style.listBackgroundColor
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

            Text {
                text: "<"
                font.family: "Poppins"
                font.pointSize: 24
                anchors.centerIn: parent
                color: "white"
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.headerBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.headerBackgroundColor, 1.2) : style.headerBackgroundColor)
            }

        }

        Text {
            text: mainClient.current_board_name
            font.family: "Poppins"
            font.pointSize: 20
            color: "white"
            anchors.centerIn: parent
        }

        Button {
            id: newListBtn

            width: 110
            height: 36
            onClicked: {
                createListPopup.open();
            }
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            Text {
                text: "+ New list"
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
            anchors.right: filterBtn.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.topMargin: 10
            font.pointSize: 16
            font.family: "Poppins"
            font.bold: true
            clip: true

            background: Rectangle {
                implicitWidth: 120
                implicitHeight: 36
                radius: style.defaultRadius
                color: style.textFormColor
                border.color: style.listBackgroundColor
                border.width: 1
            }

        }

        Button {
            id: filterBtn

            width: 36
            height: 36
            onClicked: {
                mainClient.set_filter(filterHolder.text);
                for (var i = 0; i < listview.model.count; ++i) {
                    listview.itemAtIndex(i).update_filter();
                }
            }
            anchors.right: newListBtn.left
            anchors.verticalCenter: parent.verticalCenter

            Text {
                id: filter

                text: "flt"
                font.family: "Poppins"
                font.pointSize: 16
                color: "white"
                anchors.centerIn: parent
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.headerBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.headerBackgroundColor, 1.2) : style.headerBackgroundColor)
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

        ListView {
            id: listview

            anchors.fill: parent
            anchors.leftMargin: 10
            model: mainClient.current_board
            orientation: ListView.Horizontal
            spacing: 30

            delegate: List {
            }

        }

    }

}
