import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client
import "qrc:"

Component {
    Rectangle {
        id: listMain 
        signal deleteRequest(int index)
        y: 20 
        width: style.listWidth
        height: Math.max(90, Math.min((listmodel.count+1) * 45,mainarea.height-40))
        color: style.listBackgroundColor
        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: style.headerHeight 
            color: style.primaryColor
            id: listheader
            Text {
                text: name
                color: "white"
                font.family: "Courier"
                font.pointSize: 16
                anchors.centerIn: listheader
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
                    border.width: 5
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
                                listmodel.add_card(card_name.text,card_description.text)
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
                anchors.left: listheader.left
                onClicked: {
                    create_card.open()
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
                anchors.right: listheader.right
                onClicked: deleteRequest(index)
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
        Component.onCompleted: {
            deleteRequest.connect(listview.model.delete_list);
        }
    }
}