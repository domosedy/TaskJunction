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

    radius: style.defaultRadius

    Text {
        text: name
        font.family: "Courier"
        font.pointSize: 16
        color: "white"
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
        clip: true
    }

    Button {
        height: style.smallButtonSize
        width: style.smallButtonSize
        anchors.right: parent.right
        anchors.top: parent.top
        Text {
            anchors.centerIn: parent
            text: "="
            color: "white"
        }
        onClicked: {
            card_window.open();
        }

        background: Rectangle {
            radius: style.defaultRadius
            color: parent.down ? Qt.lighter(style.cardBackgroundColor, 1.4) : (parent.hovered ? Qt.lighter(style.cardBackgroundColor, 1.2) : style.cardBackgroundColor)
        }

    }

    Popup {
        id: card_window

        width: style.cardPopUpWidth
        height: style.cardPopUpHeight
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: content

            border.color: style.primaryColor
            border.width: style.defaultBorderSize
            Drag.active: true

            TextInput {
                id: card_name

                z: 1
                text: name
                font.family: "Courier"
                font.pointSize: 12
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 10
                width: parent.width - 20
                wrapMode: Text.Wrap
                readOnly: true
                onAccepted: {
                    card_name.readOnly = true;
                    n_area.enabled = true;
                    card_name.focus = false;
                    listMain.updateCardName(index, card_name.text);
                }

                MouseArea {
                    id: n_area

                    anchors.fill: parent
                    onDoubleClicked: {
                        card_name.readOnly = false;
                        n_area.enabled = false;
                        card_name.cursorPosition = 0;
                    }
                }
                // Issue when closing popup without saving

            }

            TextInput {
                id: card_description

                z: 1
                text: description
                font.family: "Courier"
                font.pointSize: 10
                anchors.left: parent.left
                anchors.top: card_name.bottom
                anchors.topMargin: 10
                anchors.leftMargin: 10
                width: parent.width - 20
                wrapMode: Text.Wrap
                readOnly: true
                onAccepted: {
                    card_description.readOnly = true;
                    d_area.enabled = true;
                    card_description.focus = false;
                    listMain.updateCardDescription(index, card_description.text);
                }

                MouseArea {
                    id: d_area

                    anchors.fill: parent
                    onDoubleClicked: {
                        card_description.readOnly = false;
                        d_area.enabled = false;
                    }
                }

            }

            Button {
                z: 1
                width: 80
                height: 30
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                onClicked: {
                    card_window.close();
                    listMain.deleteRequest(index);
                }

                Text {
                    text: "Delete"
                    font.family: "Courier"
                    font.pointSize: 12
                    anchors.centerIn: parent
                }

                background: Rectangle {
                    color: parent.down ? Qt.lighter(style.deleteBackgroundColor, 1.2) : (parent.hovered ? Qt.lighter(style.deleteBackgroundColor, 1.2) : style.deleteBackgroundColor)
                }

            }

            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }

        }

    }

}
