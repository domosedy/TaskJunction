import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client
import "qrc:"


Rectangle {

    id: board
    visible: true

    Popup {
        id: createListPopup
        width: 210
        height: 120
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        background: Rectangle {
            visible: false
        }
        contentItem: Rectangle{
            id: content
            border.color: style.primaryColor
            border.width: style.defaultBorderSize
            TextField {
                z: 1
                id: listName
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
                height: style.smallButtonSize
                width: height*2.5
                Text {
                    text: "Add"
                    font.family: "Courier"
                    font.pointSize: 12
                    anchors.centerIn: parent
                }
                background: Rectangle {
                    color: parent.down ? Qt.lighter(style.createBackgroundColor, 1.4) :
                    (parent.hovered ? Qt.lighter(style.createBackgroundColor, 1.2) : style.createBackgroundColor)
                }     
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                onClicked: {
                    createListPopup.close()
                    mainClient.create_list(listName.text)
                    listName.text = ""
                }
            }      

            Drag.active: true
            MouseArea{
                anchors.fill: parent
                drag.target: parent
            }                             
        }     
    }

    Rectangle {
        id: boardMenuBar
        visible:true
        width: root.width
        height: style.headerHeight
        color: style.primaryColor
        anchors.top: root.top
        z: 1
        Row {
            anchors.fill: parent
            z: 1
            Button {
                width: style.smallButtonSize
                height: width
                Text {
                    text: "<"
                    font.family: "Courier"
                    font.pointSize: 12
                    anchors.centerIn: parent
                    color: "white"
                }
                background: Rectangle {
                    color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                    (parent.hovered ? Qt.darker(style.primaryColor, 1.2) : style.primaryColor)
                }     
                onClicked: {
                    loader.active = false
                    start_menu.visible = true
                }
            }                
            Button {
                width: style.smallButtonSize
                height: width
                Text {
                    anchors.centerIn: parent
                    text: "+"
                    color: "white"
                }
                background: Rectangle {
                    color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                            (parent.hovered ? Qt.darker(style.primaryColor, 1.2) : style.primaryColor)
                }
                onClicked: {
                    createListPopup.open()
                }
            }   
        }
         
    }
    Rectangle {
        id: mainarea
        width: root.width
        height: root.height-boardMenuBar.height
        y: boardMenuBar.height
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
