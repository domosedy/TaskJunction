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
        color: "white"
        width: style.cardWidth
        height: style.cardHeight
        Text {  
            text: name
            font.family: "Courier"
            font.pointSize: 16
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 5
            clip: true
        }
        MouseArea {
            anchors.fill: parent
            onDoubleClicked: card_window.open()
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
                border.color: style.primaryColor
                border.width: style.defaultBorderSize
                Text {
                    id: card_name
                    text: name
                    font.family: "Courier"
                    font.pointSize: 12
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    width: parent.width-20
                    wrapMode: Text.Wrap
                }
                Text {
                    text: description
                    font.family: "Courier"
                    font.pointSize: 10
                    anchors.left: parent.left
                    anchors.top: card_name.bottom
                    anchors.topMargin: 10
                    anchors.leftMargin: 10
                    width: parent.width-20
                    wrapMode: Text.Wrap
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
                        color: parent.down ? Qt.lighter(style.deleteBackgroundColor, 1.2) :
                        (parent.hovered ? Qt.lighter(style.deleteBackgroundColor, 1.2) : style.deleteBackgroundColor)
                    }     
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    onClicked: {
                        card_window.close()
                        //deleteRequest(index)
                        mainClient.deleteCard(listMain.index, index) // TODO: test if it works as expected
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
  