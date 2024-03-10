import QtQuick
import QtQuick.Controls
import Client
import "qrc:"


Rectangle {

    id: main
    visible: true

    Rectangle {
        id: topmenu
        visible:true
        width: root.width
        height: style.headerHeight
        color: style.primaryColor
        anchors.top: root.top
        z: 100

        Popup {
            id: create_list
            width: 210
            height: 120
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

            background: Rectangle {
                visible: false
            }
            contentItem: Rectangle{
                id: content
                border.color: style.primaryColor
                border.width: 5
                TextField {
                    z: 1
                    id: list_name
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
                    width: 80
                    height: 30
                    Text {
                        text: "Add"
                        font.family: "Courier"
                        font.pointSize: 12
                        anchors.centerIn: parent
                    }
                    background: Rectangle {
                        color: parent.down ? Qt.lighter(style.createBackgroundColor, 1.2) :
                        (parent.hovered ? Qt.lighter(style.createBackgroundColor, 1.2) : style.createBackgroundColor)
                    }     
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    onClicked: {
                        create_list.close()
                        //listview.model.add_list(list_name.text)
                        mainClient.create_list(list_name.text)
                    }
                }

                Drag.active: true
                MouseArea{
                    anchors.fill: parent
                    drag.target: parent
                }                  
            }
        }

        Button {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            height: topmenu.height
            width: height
            Text {
                anchors.centerIn: parent
                text: "+"
                color: "white"
            }
            background: Rectangle {
                color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                        (parent.hovered ? Qt.darker(style.primaryColor, 1.4) : style.primaryColor)
            }
            onClicked: {
                create_list.open()
                //listview.model.add_list()
            }
        }
    }
    Rectangle {
        id: mainarea
        width: root.width
        height: root.height-topmenu.height
        y: topmenu.height
        color: style.boardBackgroundColor


        ListView {
            id: listview
            anchors.fill: parent
            anchors.leftMargin: 10
            model: mainClient.current_board
            orientation: ListView.Horizontal
            spacing: 5
            delegate: List {}
        }
    }
}

