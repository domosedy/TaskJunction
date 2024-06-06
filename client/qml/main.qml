import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client
import "qrc:"

ApplicationWindow {
    title: "TaskJunction"
    width: 640
    height: 480
    visible: true
    id: root

    Style {
        id: style
    }

    Loader {
        id: loader
    }

    ColumnLayout{
        id: start_menu
        visible: true
        anchors.centerIn: parent
        spacing: 10
        Button {
            Layout.preferredWidth: root.width / 6
            Layout.preferredHeight: root.height / 12
            Text {
                text: "Remote"
                anchors.centerIn: parent
                font.family: "Helvetica"
                font.pointSize: 12
                color: "#514e92"
            }
            onClicked: {
                start_menu.visible = false
                if (!mainClient.is_authorized) {
                    loader.source = "Authorization.qml"
                }
                else {
                    mainClient.prepare_remote_board_select_menu();
                    loader.source = "BoardSelect.qml"
                }
                loader.active = true
            }            
        }

        Button {  
            Layout.preferredWidth: root.width / 6
            Layout.preferredHeight: root.height / 12                
            Text {
                text: "Local"
                anchors.centerIn: parent
                font.family: "Helvetica"
                font.pointSize: 12
                color: "#514e92"
            }
            onClicked: {
                start_menu.visible = false   
                mainClient.prepare_local_board_select_menu()
                loader.source = "BoardSelect.qml"
                loader.active = true
            }
        } 
        Button {  
            Layout.preferredWidth: root.width / 6
            Layout.preferredHeight: root.height / 12                
            Text {
                text: "Exit"
                anchors.centerIn: parent
                font.family: "Helvetica"
                font.pointSize: 12
                color: "white"
            }
            background: Rectangle {
                color: parent.down ? Qt.darker(style.deleteBackgroundColor, 1.4) :
                        (parent.hovered ? Qt.darker(style.deleteBackgroundColor, 1.2) : style.deleteBackgroundColor)
            }            
            onClicked: {
                Qt.quit();
            }
        }                   
    }
}
