import Client
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:"

Rectangle {
    id: root

    required property Item dragParent
    property int visualIndex: -1
    property int modelIndex: -1
    property int listIndex_: -1
    property string name_: name
    property string description_: description

    signal pressed()
    signal released()
    signal clicked()

    z: mouseArea.drag.active ? 2 : 1
    Drag.active: mouseArea.drag.active
    Drag.source: root
    Drag.hotSpot.x: root.width / 2
    Drag.hotSpot.y: root.height / 2
    radius: style.defaultRadius
    states: [
        State {
            when: mouseArea.drag.active

            ParentChange {
                target: root
                parent: root.dragParent
            }

            AnchorChanges {
                target: root
                anchors.horizontalCenter: undefined
                anchors.verticalCenter: undefined
            }

        }
    ]

    anchors {
        horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        drag.target: root
        onClicked: root.clicked()
        onPressed: {
            root.pressed();
        }
        onReleased: {
            parent.Drag.drop();
            root.released();
        }
    }

    TextInput {
        id: nameHolder

        width: parent.width - 30
        text: name
        font.family: "Poppins"
        font.pointSize: 16
        font.bold: true
        color: "white"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 20
        clip: true
        readOnly: true
        onEditingFinished: {
            nameHolder.readOnly = true;
            nameArea.enabled = true;
            mainClient.update_card(listIndex_, modelIndex, "name", nameHolder.text);
        }

        MouseArea {
            id: nameArea

            anchors.fill: parent
            onClicked: {
                nameHolder.readOnly = false;
                nameArea.enabled = false;
                nameHolder.cursorPosition = 0;
                nameHolder.focus = true;
            }
        }

    }

    TextInput {
        id: descriptionHolder

        width: parent.width - 30
        height: parent.height - 70
        text: description
        font.family: "Poppins"
        font.pointSize: 12
        color: "white"
        anchors.top: nameHolder.bottom
        anchors.left: parent.left
        anchors.topMargin: 5
        anchors.leftMargin: 20
        readOnly: true
        wrapMode: TextInput.Wrap
        onEditingFinished: {
            descriptionHolder.readOnly = true;
            descriptionArea.enabled = true;
            mainClient.update_card(listIndex_, modelIndex, "description", descriptionHolder.text);
        }

        MouseArea {
            id: descriptionArea

            anchors.fill: parent
            onClicked: {
                descriptionHolder.readOnly = false;
                descriptionArea.enabled = false;
                descriptionHolder.cursorPosition = 0;
                descriptionHolder.focus = true;
            }
        }

    }

    Rectangle {
        id: tags

        width: parent.width - 30
        height: 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15
        anchors.leftMargin: 20
        color: style.cardBackgroundColor

        ListView {
            id: tagList

            anchors.fill: parent
            model: cardModel
            orientation: ListView.Horizontal
            spacing: 2
            clip: true

            footerPositioning: ListView.OverlayFooter
            footer: Button {
                width: 18
                height: 18

                onClicked: {
                    mainClient.create_tag(listIndex_, modelIndex,"lol")
                }

                Text {
                    text: "+"
                    font.family: "Poppins"
                    font.pointSize: 16
                    anchors.centerIn: parent
                    color: "white"
                }

                background: Rectangle {
                    color: parent.down ? Qt.darker(style.listBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.listBackgroundColor, 1.2) : style.listBackgroundColor)
                }
            }

            delegate: Rectangle {
                id: tag
                z: 1

                Text {
                    text: name
                    font.family: "Poppins"
                    font.pointSize: 16
                    anchors.centerIn: parent
                    color: "white"
                }            

                color: style.listBackgroundColor
                radius: style.defaultBorderSize    
            }
        }
    }

    Button {
        id: deleteCardButton

        width: 18
        height: 18
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 20
        anchors.topMargin: 20
        onClicked: {
            listMain.deleteRequest(index);
        }

        Text {
            text: "x"
            font.family: "Poppins"
            font.pointSize: 16
            anchors.centerIn: parent
            color: "white"
        }

        background: Rectangle {
            color: parent.down ? Qt.darker(style.cardBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.cardBackgroundColor, 1.2) : style.cardBackgroundColor)
        }

    }

}
