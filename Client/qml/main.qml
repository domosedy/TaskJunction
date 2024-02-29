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
        id: client_loader
    }

    ColumnLayout{
        id: login
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
                login.visible = false
                client_loader.source = "Client.qml"   
                mainClient.create_board()
            }
        }      
    }
}