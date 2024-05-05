import Client
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:"

Rectangle {
    id: listMain

    function deleteRequest(card_index: int) {
        mainClient.delete_card(index, card_index);
    }

    function updateCardName(card_index: int, new_name: string) {
        mainClient.update_card_name(index, card_index, new_name);
    }

    function updateCardDescription(card_index: int, new_desctiption: string) {
        mainClient.update_card_description(index, card_index, new_desctiption);
    }

    width: style.listWidth
    height: Math.min(root.height - 80, 70+(listmodel.count)*(style.cardHeight+style.cardSpacing))
    color: style.listBackgroundColor

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
            Drag.active: true

            ColumnLayout {
                z: 1
                width: parent.width
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10

                TextField {
                    id: card_name

                    z: 1
                    placeholderText: "New card"
                    font.family: "Courier"
                    font.pointSize: 12
                    font.bold: true
                    Layout.preferredWidth: parent.width - 20
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                }

                TextField {
                    id: card_description

                    z: 1
                    placeholderText: "Description"
                    font.family: "Courier"
                    font.pointSize: 12
                    font.bold: true
                    Layout.preferredWidth: parent.width - 20
                    Layout.alignment: Qt.AlignHCenter
                }

                Button {
                    z: 1
                    Layout.preferredWidth: parent.width - 40
                    Layout.preferredHeight: 30
                    Layout.margins: 20
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                    onClicked: {
                        create_card.close();
                        mainClient.create_card(index, card_name.text, card_description.text);
                        card_description.text = "";
                        card_name.text = "";
                    }

                    Text {
                        text: "Add"
                        font.family: "Courier"
                        font.pointSize: 12
                        anchors.centerIn: parent
                    }

                    background: Rectangle {
                        color: parent.down ? Qt.lighter(style.createBackgroundColor, 1.2) : (parent.hovered ? Qt.lighter(style.createBackgroundColor, 1.2) : style.createBackgroundColor)
                    }

                }

            }

            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }

        }

    }

    Rectangle {
        id: listcontent

        width: parent.width
        height: listMain.height
        color: style.listBackgroundColor

        ListView {
            id: thislist

            clip: true          
            anchors.fill: parent
            spacing: style.cardSpacing         

            header: Rectangle {
                id: listheader
                z: 3
                width: style.listWidth
                height: style.listHeaderHeight
                color: style.listBackgroundColor

                Row {
                    anchors.fill: parent
                    layoutDirection: Qt.LeftToRight

                    Button {
                        height: listheader.height
                        width: height
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: {
                            create_card.open();
                        }

                        Text {
                            anchors.centerIn: parent
                            text: "+"
                            color: "white"
                        }

                        background: Rectangle {
                            color: parent.down ? Qt.darker(style.listBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.listBackgroundColor, 1.4) : style.listBackgroundColor)
                        }

                    }

                    TextInput {
                        id: list_name

                        z: 1
                        text: name
                        font.family: "Courier"
                        font.pointSize: 16
                        color: "white"
                        width: parent.width - 2 * listheader.height
                        anchors.verticalCenter: parent.verticalCenter
                        clip: true
                        readOnly: true
                        onAccepted: {
                            list_name.readOnly = true;
                            ln_area.enabled = true;
                            list_name.focus = false;
                            mainClient.update_list_name(index, list_name.text);
                        }

                        MouseArea {
                            id: ln_area

                            anchors.fill: parent
                            onDoubleClicked: {
                                list_name.readOnly = false;
                                ln_area.enabled = false;
                                list_name.cursorPosition = 0;
                            }
                        }

                    }

                    Button {
                        height: listheader.height
                        width: height
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: mainClient.delete_list(index)

                        Text {
                            anchors.centerIn: parent
                            text: "-"
                            color: "white"
                        }

                        background: Rectangle {
                            color: parent.down ? Qt.darker(style.listBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.listBackgroundColor, 1.4) : style.listBackgroundColor)
                        }

                    }

                }

            }
            headerPositioning: ListView.OverlayHeader

            footer: Rectangle {
                id: listFooted

                width: listcontent.width
                height: 30
                //color: style.boardBackgroundColor
                color: "red"

                DropArea {
                    id: footedDrop

                    anchors.fill: parent
                    onDropped: function(drag) {
                        var from = drag.source.modelIndex;
                        drag.source.dragParent.clip = true;
                        mainClient.move(from, -1, drag.source.listIndex_, listIndex);
                    }
                }

            }

            model: DelegateModel {
                id: listVisualModel

                model: listmodel

                delegate: DropArea {
                    id: delegateRoot

                    property string name_: name
                    property string description_: description
                    property int visualIndex: DelegateModel.itemsIndex
                    property int parentListIndex: listIndex
                    property int modelIndex: cardIndex

                    width: style.cardWidth
                    height: style.cardHeight
                    anchors.leftMargin: 10
                    onEntered: function(drag) {
                        var from = drag.source.visualIndex;
                        var to = cardRoot.visualIndex;
                        if (drag.source.listIndex_ == cardRoot.listIndex_) {
                            listVisualModel.items.move(from, to);
                        }
                    }
                    onDropped: function(drag) {
                        var from = drag.source.modelIndex;
                        var to = (drag.source.listIndex_ != cardRoot.listIndex_) ? visualIndex : drag.source.visualIndex;
                        drag.source.dragParent.clip = true;
                        mainClient.move(from, to, drag.source.listIndex_, cardRoot.listIndex_);
                    }

                    Card {
                        id: cardRoot

                        width: style.cardWidth
                        height: style.cardHeight
                        color: style.cardBackgroundColor
                        
                        dragParent: thislist
                        visualIndex: delegateRoot.visualIndex
                        modelIndex: delegateRoot.modelIndex
                        listIndex_: parentListIndex
                        onPressed: {
                            thislist.clip = false
                        }
                    }

                }

            }

            displaced: Transition {
                NumberAnimation {
                    properties: "x,y"
                    easing.type: Easing.OutQuad
                }

            }

        }

    }

}
