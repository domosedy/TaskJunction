import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client
import "qrc:"

Component {
    id: card
    Rectangle {
        id: card_main
        signal deleteRequest(int index)
        color: "#ffffff"
        width: listMain.width-10
        height: 40
        Text {  
            text: name
            font.family: "Courier"
            font.pointSize: 16
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 5
        }
        MouseArea {
            anchors.fill: parent
            onClicked: card_window.open()
        }
        Popup {
            id: card_window
            width: root.width/2
            height: root.height/2
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

            background: Rectangle {
                visible: false
            }
            contentItem: Rectangle{
                id: content
                border.color: "#282c34"
                border.width: 5
                Text {
                    id: card_name
                    text: name
                    font.family: "Courier"
                    font.pointSize: 12
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 10
                }
                Text {
                    text: description
                    font.family: "Courier"
                    font.pointSize: 10
                    anchors.left: parent.left
                    anchors.top: card_name.bottom
                    anchors.topMargin: 10
                    anchors.leftMargin: 10
                }                
                Button {
                    z: 1
                    width: 80
                    height: 30
                    Text {
                        text: "Delete"
                        font.family: "Courier"
                        font.pointSize: 12
                        anchors.centerIn: parent
                    }
                    background: Rectangle {
                        color: parent.down ? "#ff9999" :
                        (parent.hovered ? "#ff9999" : "#ff6666")
                    }     
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    onClicked: {
                        card_window.close()
                        deleteRequest(index)
                    }
                }

                Drag.active: true
                MouseArea{
                    anchors.fill: parent
                    drag.target: parent
                }                  
            }
        }
        Component.onCompleted: {
            deleteRequest.connect(listmodel.delete_card);
        }
    }
}     