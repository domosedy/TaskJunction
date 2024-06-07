import Client
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:"

ApplicationWindow {
    id: root

    title: "TaskJunction"
    width: 1280
    height: 720
    visible: true
    color: style.primaryColor

    Style {
        id: style
    }

    Loader {
        id: loader
    }

    ColumnLayout {
        id: startMenu

        visible: true
        anchors.centerIn: parent
        spacing: 10

        Rectangle {
            id: background

            color: style.primaryColor
        }

        Button {
            Layout.preferredWidth: root.width / 6
            Layout.preferredHeight: root.height / 12
            onClicked: {
                startMenu.visible = false;
                loader.source = "Authorization.qml";
                loader.active = true;
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

        Button {
            Layout.preferredWidth: root.width / 6
            Layout.preferredHeight: root.height / 12
            onClicked: {
                startMenu.visible = false;
                mainClient.load_local_boards();
                loader.source = "BoardSelect.qml";
                loader.active = true;
            }

            Text {
                anchors.centerIn: parent
                text: "Boards"
                color: "white"
                font.pointSize: 16
                font.family: "Poppins"
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.boardBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.boardBackgroundColor, 1.2) : style.boardBackgroundColor)
                radius: style.defaultRadius
            }

        }

        Button {
            Layout.preferredWidth: root.width / 6
            Layout.preferredHeight: root.height / 12
            onClicked: {
                Qt.quit();
            }

            Text {
                anchors.centerIn: parent
                text: "Exit"
                color: "black"
                font.pointSize: 16
                font.family: "Poppins"
            }

            background: Rectangle {
                // color: parent.down ? Qt.darker(style.deleteBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.deleteBackgroundColor, 1.2) : style.deleteBackgroundColor)
                color: parent.down ? Qt.darker("#f5f5f5", 1.4) : (parent.hovered ? Qt.darker("#f5f5f5", 1.2) : "#f5f5f5")
                radius: style.defaultRadius
            }

        }

    }

}
