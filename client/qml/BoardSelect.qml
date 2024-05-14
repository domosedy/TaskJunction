import Client
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:"

Rectangle {
    id: boardSelect

    width: root.width
    height: root.height
    color: style.primaryColor

    Popup {
        id: connectBoardPopUp

        width: 210
        height: 160
        x: root.width / 2
        y: root.height / 2
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        focus: true

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: boardPopUpContent

            border.color: style.listBackgroundColor
            border.width: style.defaultBorderSize
            color: style.listBackgroundColor
            Drag.active: true

            TextField {
                id: groupLink

                z: 1
                placeholderText: "Enter board link"
                placeholderTextColor: "#bfbfbf"
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                font.pointSize: 16
                font.family: "Poppins"

                background: Rectangle {
                    implicitWidth: boardPopUpContent.width - 20
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
                    connectBoardPopUp.close();
                    mainClient.connect_board(groupLink.text);
                    groupLink.text = "";
                }

                Text {
                    text: "Connect"
                    font.family: "Poppins"
                    font.pointSize: 16
                    anchors.centerIn: parent
                    color: "white"
                }

                background: Rectangle {
                    color: parent.down ? Qt.lighter(style.primaryColor, 1.2) : (parent.hovered ? Qt.lighter(style.primaryColor, 1.2) : style.primaryColor)
                }

            }

            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }

        }

    }

    Popup {
        id: createBoardPopup

        width: 210
        height: 210
        x: root.width / 2
        y: root.height / 2
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        focus: true

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: createBoardContent

            border.color: style.primaryColor
            border.width: style.defaultBorderSize
            color: style.listBackgroundColor
            Drag.active: true

            TextField {
                id: boardName

                z: 1
                placeholderText: "Enter name"
                placeholderTextColor: "#bfbfbf"
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                font.pointSize: 16
                font.family: "Poppins"

                background: Rectangle {
                    implicitWidth: createBoardContent.width - 20
                    implicitHeight: 36
                    radius: style.defaultRadius
                    color: style.textFormColor
                    border.color: style.listBackgroundColor
                    border.width: 1
                }

            }

            TextField {
                id: boardDescription

                z: 1
                placeholderText: "Enter description"
                placeholderTextColor: "#bfbfbf"
                anchors.top: boardName.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                font.pointSize: 16
                font.family: "Poppins"

                background: Rectangle {
                    implicitWidth: createBoardContent.width - 20
                    implicitHeight: 36
                    radius: style.defaultRadius
                    color: style.textFormColor
                    border.color: style.listBackgroundColor
                    border.width: 1
                }

            }

            // TODO make better
            ComboBox {
                id: boardType

                z: 1
                implicitWidth: parent.width - 20
                implicitHeight: 36
                anchors.top: boardDescription.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                anchors.bottomMargin: 10
                Component.onCompleted: {
                    if (mainClient.connection_status != Client.Authorized)
                        model.append({
                        "text": "Remote"
                    });

                }

                model: ListModel {
                    id: typeModel

                    ListElement {
                        text: "Local"
                    }

                }

                background: Rectangle {
                    color: parent.down ? Qt.lighter(style.textFormColor, 1.2) : (parent.hovered ? Qt.lighter(style.textFormColor, 1.2) : style.textFormColor)
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
                    createBoardPopup.close();
                    mainClient.create_board(boardName.text, boardDescription.text, boardType.currentText);
                    boardDescription.text = "";
                    boardName.text = "";
                }

                Text {
                    text: "Create"
                    font.family: "Poppins"
                    font.pointSize: 16
                    anchors.centerIn: parent
                    color: "white"
                }

                background: Rectangle {
                    color: parent.down ? Qt.lighter(style.primaryColor, 1.2) : (parent.hovered ? Qt.lighter(style.primaryColor, 1.2) : style.primaryColor)
                }

            }

            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }

        }

    }

    Rectangle {
        id: boardSelectHeader

        visible: true
        width: root.width
        height: style.headerHeight
        color: style.headerBackgroundColor
        anchors.top: root.top

        Button {
            id: toMenuButton

            width: style.headerHeight
            height: style.headerHeight
            anchors.left: parent.left
            onClicked: {
                loader.active = false;
                startMenu.visible = true;
            }

            Text {
                text: "<"
                font.family: "Poppins"
                font.pointSize: 24
                anchors.centerIn: parent
                color: "white"
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.headerBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.headerBackgroundColor, 1.2) : style.headerBackgroundColor)
            }

        }

        Text {
            text: "Welcome, master ^_^"
            font.family: "Poppins"
            font.pointSize: 20
            color: "white"
            anchors.centerIn: parent
        }

        Button {
            width: 136
            height: style.headerHeight / 2
            anchors.top: parent.top
            anchors.right: parent.right
            onClicked: {
                createBoardPopup.open();
            }

            Text {
                text: "New board"
                font.family: "Poppins"
                font.pointSize: 16
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10
                color: "white"
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.headerBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.headerBackgroundColor, 1.2) : style.headerBackgroundColor)
            }

        }

        Button {
            width: 136
            height: style.headerHeight / 2
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            //visible: (mainClient.connection_status == Client.Authorized)
            visible: true
            onClicked: {
                connectBoardPopUp.open();
            }

            Text {
                text: "Connect"
                font.family: "Poppins"
                font.pointSize: 16
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10
                color: "white"
            }

            background: Rectangle {
                color: parent.down ? Qt.darker(style.headerBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.headerBackgroundColor, 1.2) : style.headerBackgroundColor)
            }

        }

    }

    Rectangle {
        id: boardSelectMain

        width: root.width
        height: root.height - boardSelectHeader.height
        y: boardSelectHeader.height
        color: style.primaryColor

        ListView {
            id: boardsList

            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.topMargin: 30
            model: mainClient.board_menu
            orientation: ListView.Vertical
            spacing: 5
            clip: true

            delegate: Rectangle {
                id: boardCard

                width: 280
                height: 160
                color: style.boardCardColor
                radius: style.defaultRadius
                anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

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
                        mainClient.update_board(index, "name", nameHolder.text);
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
                        mainClient.update_board(index, "description", descriptionHolder.text);
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

                Button {
                    id: deleteBoardButton

                    width: 18
                    height: 18
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.rightMargin: 20
                    anchors.topMargin: 20
                    onClicked: {
                        mainClient.delete_board(index);
                    }

                    Text {
                        text: "x"
                        font.family: "Poppins"
                        font.pointSize: 16
                        anchors.centerIn: parent
                        color: "white"
                    }

                    background: Rectangle {
                        color: parent.down ? Qt.darker(style.boardCardColor, 1.4) : (parent.hovered ? Qt.darker(style.boardCardColor, 1.2) : style.boardCardColor)
                    }

                }

                Button {
                    id: shareBoardButton

                    width: 18
                    height: 18
                    anchors.right: parent.right
                    anchors.top: deleteBoardButton.bottom
                    anchors.rightMargin: 20
                    anchors.topMargin: 10
                    onClicked: {
                        console.log("Lol there is nothing yet");
                    }

                    Text {
                        text: "@"
                        font.family: "Poppins"
                        font.pointSize: 12
                        anchors.centerIn: parent
                        color: "white"
                    }

                    background: Rectangle {
                        color: parent.down ? Qt.darker(style.boardCardColor, 1.4) : (parent.hovered ? Qt.darker(style.boardCardColor, 1.2) : style.boardCardColor)
                    }

                }

                Button {
                    width: parent.width - 40
                    height: 48
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        mainClient.load_board(index);
                        loader.source = "Board.qml";
                    }

                    Text {
                        text: "View board"
                        font.family: "Poppins"
                        font.pointSize: 16
                        font.bold: true
                        anchors.centerIn: parent
                        color: "white"
                    }

                    background: Rectangle {
                        color: parent.down ? Qt.lighter(style.primaryColor, 1.4) : (parent.hovered ? Qt.lighter(style.primaryColor, 1.2) : style.primaryColor)
                        radius: style.defaultRadius
                    }

                }

            }

        }

    }

}
