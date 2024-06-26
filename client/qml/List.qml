import Client
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:"

Rectangle {
    id: listMain

    property int visibleCount: listmodel.count

    function deleteRequest(card_index: int) {
        mainClient.delete_card(index, card_index);
    }

    function updateCardName(card_index: int, new_name: string) {
        mainClient.update_card(index, card_index, "name", new_name);
    }

    function updateCardDescription(card_index: int, new_description: string) {
        mainClient.update_card(index, card_index, "description", new_description);
    }

    function update_filter(filter: string) {
        if (filter != "")
            listVisualModel.filterOnGroup = "filtered";
        else
            listVisualModel.filterOnGroup = "all";
        listVisualModel.update();
    }

    width: style.listWidth
    height: Math.min(150 + 3 * (style.cardHeight + style.cardSpacing), 150 + (visibleCount) * (style.cardHeight + style.cardSpacing))
    color: style.listBackgroundColor
    radius: style.defaultRadius

    Connections {
        function onCountChanged() {
            if (listVisualModel.filterOnGroup == "all")
                visibleCount = listmodel.count;

        }

        target: listmodel
    }

    Popup {
        id: createCardPopup

        width: 210
        height: 160
        z: 4
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        focus: true
        x: 0
        y: listMain.height - 160

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: createCardContent

            border.color: style.primaryColor
            border.width: style.defaultBorderSize
            color: style.listBackgroundColor
            radius: style.defaultRadius
            Drag.active: true

            TextField {
                id: cardName

                z: 1
                placeholderText: "Enter name"
                placeholderTextColor: "#bfbfbf"
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                font.pointSize: 16
                font.family: "Poppins"
                color: "white"

                background: Rectangle {
                    implicitWidth: createCardContent.width - 20
                    implicitHeight: 36
                    radius: style.defaultRadius
                    color: style.textFormColor
                    border.color: style.listBackgroundColor
                    border.width: 1
                }

            }

            TextField {
                id: cardDescription

                z: 1
                placeholderText: "Enter description"
                placeholderTextColor: "#bfbfbf"
                anchors.top: cardName.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                font.pointSize: 16
                font.family: "Poppins"
                color: "white"

                background: Rectangle {
                    implicitWidth: createCardContent.width - 20
                    implicitHeight: 36
                    radius: style.defaultRadius
                    color: style.textFormColor
                    border.color: style.listBackgroundColor
                    border.width: 1
                }

            }

            Button {
                z: 1
                implicitWidth: parent.width - 20
                implicitHeight: 36
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 10
                onClicked: {
                    createCardPopup.close();
                    mainClient.create_card(index, cardName.text, cardDescription.text);
                    cardDescription.text = "";
                    cardName.text = "";
                }

                Text {
                    text: "Add"
                    font.family: "Poppins"
                    font.pointSize: 16
                    anchors.centerIn: parent
                    color: "white"
                }

                background: Rectangle {
                    color: parent.down ? Qt.lighter(style.primaryColor, 1.2) : (parent.hovered ? Qt.lighter(style.primaryColor, 1.2) : style.primaryColor)
                    radius: style.defaultRadius
                }

            }

            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }

        }

    }

    Rectangle {
        id: listContent

        width: parent.width
        height: listMain.height
        color: style.listBackgroundColor
        radius: style.defaultRadius

        ListView {
            id: thisList

            clip: true
            anchors.fill: parent
            spacing: style.cardSpacing
            headerPositioning: ListView.OverlayHeader
            footerPositioning: ListView.OverlayFooter

            header: Rectangle {
                id: listHeader

                z: 3
                width: style.listWidth
                height: style.listHeaderHeight
                color: style.listBackgroundColor
                radius: style.defaultRadius

                TextInput {
                    id: listName

                    z: 1
                    text: name
                    font.family: "Poppins"
                    font.pointSize: 16
                    font.bold: true
                    color: "white"
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 20
                    clip: true
                    readOnly: true
                    onEditingFinished: {
                        listName.readOnly = true;
                        lnArea.enabled = true;
                        mainClient.update_list(index, listName.text);
                    }

                    MouseArea {
                        id: lnArea

                        anchors.fill: parent
                        onClicked: {
                            listName.readOnly = false;
                            lnArea.enabled = false;
                            listName.cursorPosition = 0;
                            listName.focus = true;
                        }
                    }

                }

                Button {
                    id: deleteListButton

                    width: 26
                    height: 26
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 20
                    onClicked: mainClient.delete_list(index)
                    icon.source: "bin.svg"
                    icon.width: width
                    icon.height: height

                    background: Rectangle {
                        color: parent.down ? Qt.darker(style.boardCardColor, 1.4) : (parent.hovered ? Qt.darker(style.boardCardColor, 1.2) : style.boardCardColor)
                    }

                }

            }

            footer: Rectangle {
                id: listFooted

                z: 3
                width: listContent.width
                height: 68
                color: style.listBackgroundColor
                radius: style.defaultRadius
                border.color: style.defaultMoveColor
                border.width: 0

                Button {
                    width: style.listWidth - 40
                    height: 40
                    anchors.centerIn: parent
                    onClicked: {
                        createCardPopup.open();
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "+ New card"
                        color: "white"
                        font.pointSize: 16
                        font.family: "Poppins"
                    }

                    background: Rectangle {
                        color: parent.down ? Qt.darker(style.cardBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.cardBackgroundColor, 1.2) : style.cardBackgroundColor)
                        radius: style.defaultRadius
                    }

                }

                DropArea {
                    id: footedDrop

                    anchors.fill: parent
                    onEntered: function(drag) {
                        border.width = style.moveRadius;
                    }
                    onDropped: function(drag) {
                        let from = drag.source.get_modelIndex();
                        let sourceList = drag.source.get_listIndex();
                        border.width = 0;
                        if (listIndex == sourceList)
                            listVisualModel.groups[2].move(from, listmodel.count - 1);

                        mainClient.move(from, -1, sourceList, listIndex);
                    }
                    onExited: function(drag) {
                        border.width = 0;
                    }
                }

            }

            model: DelegateModel {
                id: listVisualModel

                function update() {
                    if (allItems.count == 0)
                        return ;

                    allItems.setGroups(0, allItems.count, ["all"]);
                    for (let index = 0; index < allItems.count; index++) {
                        if (mainClient.is_filtered(listIndex, index))
                            allItems.setGroups(index, 1, ["all", "filtered"]);

                    }
                    visibleCount = visibleItems.count;
                }

                model: listmodel
                Component.onCompleted: {
                    Qt.callLater(update);
                }
                filterOnGroup: "all"
                groups: [
                    DelegateModelGroup {
                        id: allItems

                        name: "all"
                        includeByDefault: true
                        onCountChanged: Qt.callLater(update)
                    },
                    DelegateModelGroup {
                        id: visibleItems

                        name: "filtered"
                    }
                ]

                delegate: DropArea {
                    id: delegateRoot

                    property string name_: name
                    property string description_: description
                    property int parentListIndex: listIndex
                    property int visualIndex: DelegateModel.itemsIndex
                    property int modelIndex: cardIndex

                    function create_tag(name) {
                        if (listIndex != undefined && cardIndex != undefined)
                            mainClient.create_tag(listIndex, cardIndex, name);

                    }

                    function delete_tag(tag_index) {
                        if (listIndex != undefined && cardIndex != undefined)
                            mainClient.delete_tag(listIndex, cardIndex, tag_index);

                    }

                    function update_card(type, value) {
                        if (listIndex != undefined && cardIndex != undefined)
                            mainClient.update_card(listIndex, cardIndex, type, value);

                    }

                    function actual_modelIndex() {
                        return modelIndex;
                    }

                    function actual_visualIndex() {
                        return visualIndex;
                    }

                    function actual_listIndex() {
                        return listIndex;
                    }

                    width: style.cardWidth
                    height: style.cardHeight
                    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                    onEntered: function(drag) {
                        let from = drag.source.get_visualIndex();
                        let to = visualIndex;
                        if (drag.source.get_listIndex() == listIndex)
                            listVisualModel.groups[2].move(from, to);
                        else
                            cardRoot.border.color = style.defaultMoveColor;
                    }
                    onExited: function(drag) {
                        cardRoot.border.color = style.listBackgroundColor;
                    }
                    onDropped: function(drag) {
                        let from = drag.source.get_modelIndex();
                        let to = (drag.source.get_listIndex() != listIndex) ? visualIndex : drag.source.get_visualIndex();
                        cardRoot.border.color = style.listBackgroundColor;
                        mainClient.move(from, to, drag.source.get_listIndex(), listIndex);
                    }

                    Card {
                        id: cardRoot

                        width: style.cardWidth
                        height: style.cardHeight
                        color: style.cardBackgroundColor
                        dragParent: cardDragContainer
                        dragList: thisList
                        visualIndex: delegateRoot.visualIndex
                        modelIndex: delegateRoot.modelIndex
                        listIndex_: parentListIndex
                        thisCardModel: cardModel
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
