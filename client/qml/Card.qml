import Client
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:"

Rectangle {
    id: root

    required property Item dragParent
    required property Item dragList
    property int visualIndex: -1
    property int modelIndex: -1
    property int listIndex_: -1
    property string name_: name
    property string description_: description
    required property var thisCardModel

    signal pressed()
    signal released()
    signal clicked()

    function get_modelIndex() {
        return delegateRoot.actual_modelIndex();
    }

    function get_visualIndex() {
        return delegateRoot.actual_visualIndex();
    }

    function get_listIndex() {
        return delegateRoot.actual_listIndex();
    }

    z: mouseArea.drag.active ? 2 : 1
    Drag.active: mouseArea.drag.active
    Drag.source: root
    Drag.hotSpot.x: root.width / 2
    Drag.hotSpot.y: root.height / 2
    radius: style.defaultRadius
    border.width: style.defaultBorderSize + 1
    border.color: style.listBackgroundColor
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
            delegateRoot.update_card("name", nameHolder.text);
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
            delegateRoot.update_card("description", descriptionHolder.text);
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

    Popup {
        id: createTagPopUp

        width: 210
        height: 120
        x: root.width / 2 - 50
        y: root.height / 2
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        focus: true

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: tagContent

            border.color: style.cardBackgroundColor
            border.width: style.defaultBorderSize
            color: style.listBackgroundColor
            radius: style.defaultRadius
            Drag.active: true

            TextField {
                id: tagName

                z: 1
                placeholderText: "Enter tag"
                placeholderTextColor: "#bfbfbf"
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                font.pointSize: 16
                font.family: "Poppins"
                color: "white"

                background: Rectangle {
                    implicitWidth: tagContent.width - 20
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
                    createTagPopUp.close();
                    delegateRoot.create_tag(tagName.text);
                    tagName.text = "";
                }

                Text {
                    text: "New"
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
        id: tags

        width: parent.width - 30
        height: 30
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: 15
        anchors.leftMargin: 10
        color: style.primaryColor

        ListView {
            id: tagList

            anchors.fill: parent
            model: thisCardModel
            orientation: ListView.Horizontal
            spacing: 3
            clip: true
            z: 1

            footer: Button {
                width: 20
                height: 20
                onClicked: {
                    createTagPopUp.open();
                }

                Text {
                    text: "+"
                    font.family: "Poppins"
                    font.pointSize: 16
                    anchors.centerIn: parent
                    color: "white"
                }

                background: Rectangle {
                    color: parent.down ? Qt.darker(style.cardBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.cardBackgroundColor, 1.2) : style.cardBackgroundColor)
                }

            }

            delegate: Rectangle {
                id: tag

                anchors.verticalCenter: parent ? parent.verticalCenter : undefined
                height: 20
                width: 60
                z: 1
                color: style.listBackgroundColor
                radius: style.defaultRadius

                Text {
                    text: name
                    font.family: "Poppins"
                    font.pointSize: 10
                    anchors.centerIn: parent
                    color: "white"
                    clip: true
                    width: parent.width-5
                }

                MouseArea {
                    z: 3
                    anchors.fill: parent
                    onClicked: {
                        tagDelete.visible = !tagDelete.visible;
                    }
                }

                Button {
                    id: tagDelete

                    width: 20
                    height: 20
                    z: 4
                    onClicked: {
                        delegateRoot.delete_tag(index);
                    }
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    visible: false
                    icon.source: "bin.svg"
                    icon.width: width
                    icon.height: height

                    background: Rectangle {
                        color: parent.down ? Qt.darker(style.listBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.listBackgroundColor, 1.2) : style.listBackgroundColor)
                        radius: style.defaultRadius
                    }

                }

            }

        }

    }

    Button {
        id: deleteCardButton

        width: 26
        height: 26
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 20
        anchors.topMargin: 20
        onClicked: {
            listMain.deleteRequest(index);
        }
        icon.source: "bin.svg"
        icon.width: width
        icon.height: height

        background: Rectangle {
            color: parent.down ? Qt.darker(style.cardBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.cardBackgroundColor, 1.2) : style.cardBackgroundColor)
        }

    }

}
