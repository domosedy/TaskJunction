import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client
import "qrc:"

Rectangle {
    id: listMain 
    y: 20 
    width: style.listWidth
    height: Math.max(90, Math.min((listmodel.count+1) * 45,mainarea.height-40))
    color: style.listBackgroundColor

    function deleteRequest(card_index : int) {
        mainClient.delete_card(index, card_index)
    }

    function updateCardName(card_index : int, new_name : string) {
        mainClient.update_card_name(index, card_index, new_name)
    }

    function updateCardDescription(card_index : int, new_desctiption : string) {
        mainClient.update_card_description(index, card_index, new_desctiption)
    }
    
    Popup {
        id: create_card
        width: 210
        height: 160
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        background: Rectangle {
            visible: false
        }
        contentItem: Rectangle {
            id: content
            border.color: style.primaryColor
            border.width: style.defaultBorderSize
            ColumnLayout {
                z:1
                width: parent.width
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10
                TextField {
                    z: 1
                    id: card_name
                    placeholderText: "New card"
                    font.family: "Courier"
                    font.pointSize: 12
                    font.bold: true
                    Layout.preferredWidth: parent.width - 20
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                }      
                TextField {
                    z: 1
                    id: card_description
                    placeholderText: "Description"

                    font.family: "Courier"
                    font.pointSize: 12
                    font.bold: true
                    Layout.preferredWidth: parent.width - 20
                    Layout.alignment: Qt.AlignHCenter
                }                                
                Button {
                    z: 1
                    Layout.preferredWidth: parent.width-40
                    Layout.preferredHeight: 30
                    Layout.margins: 20
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
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                    onClicked: {
                        create_card.close()
                        mainClient.create_card(index, card_name.text, card_description.text)
                        card_description.text = ""
                        card_name.text = ""
                    }
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
        anchors.top: parent.top
        width: parent.width
        height: style.headerHeight 
        color: style.primaryColor
        id: listheader
        Row {   
            anchors.fill: parent 
            layoutDirection: Qt.LeftToRight
            Button {
                Text {
                    anchors.centerIn: parent
                    text: "+"
                    color: "white"
                }
                height: listheader.height
                width: height
                background: Rectangle {
                    color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                            (parent.hovered ? Qt.darker(style.primaryColor, 1.4) : style.primaryColor)
                }            
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    create_card.open()
                }
            }  
            TextInput {
                z: 1
                id: list_name
                text: name
                font.family: "Courier"
                font.pointSize: 16
                color: "white"
                width: parent.width - 2*listheader.height
                anchors.verticalCenter: parent.verticalCenter
                clip: true

                readOnly: true
                MouseArea {
                    id: ln_area
                    anchors.fill: parent
                    onDoubleClicked: {
                        list_name.readOnly = false
                        ln_area.enabled = false
                        list_name.cursorPosition = 0
                    }
                }
                onAccepted: { 
                    list_name.readOnly = true
                    ln_area.enabled = true
                    list_name.focus = false
                    mainClient.update_list_name(index, list_name.text)
                }
            }                             
            Button {
                Text {
                    anchors.centerIn: parent
                    text: "-"
                    color: "white"
                }
                height: listheader.height
                width: height
                background: Rectangle {
                    color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                            (parent.hovered ? Qt.darker(style.primaryColor, 1.4) : style.primaryColor)
                }            
                anchors.verticalCenter: parent.verticalCenter
                onClicked: mainClient.delete_list(index)
            }  
        }          
    }

    Rectangle {
        id: listcontent
        width: parent.width
        height: listMain.height-40
        anchors.top: listheader.bottom
        color: style.listBackgroundColor

        ListView {
            id:thislist
            clip: true
            y: 10
            anchors.fill: parent
            anchors.topMargin: 10
            anchors.leftMargin: 5
            model: listmodel
            spacing: 5
            delegate: Card {}               
        }             
    }
}
