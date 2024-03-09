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
                text: "Sign in"
                anchors.centerIn: parent
                font.family: "Helvetica"
                font.pointSize: 12
                color: "#514e92"
            }
            onClicked: {
                start_menu.visible = false
                loader.source = "Authorization.qml"   
            }            
        }

        Button {  
            Layout.preferredWidth: root.width / 6
            Layout.preferredHeight: root.height / 12                
            Text {
                text: "Local mode"
                anchors.centerIn: parent
                font.family: "Helvetica"
                font.pointSize: 12
                color: "#514e92"
            }
            onClicked: {
                start_menu.visible = false
                loader.source = "Client.qml"   
                mainClient.create_board()
            }
        }      
    }
}
