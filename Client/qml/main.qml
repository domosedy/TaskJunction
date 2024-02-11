import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    title: "TaskJunction"
    width: 640
    height: 480
    visible: true
    id: root

    Loader {
        id: client_loader
    }

    ColumnLayout{
        id: lay
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
                lay.visible = false
                client_loader.source = "client.qml"    
            }
        } 
    }
}