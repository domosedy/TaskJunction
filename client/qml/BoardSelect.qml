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
        id: connectBoardPopup

        width: 210
        height: 120
        x: root.width - 260
        y: style.headerHeight
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        focus: true
        onOpened: {
            groupLink.text = "";
        }

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: boardPopUpContent

            border.color: style.primaryColor
            border.width: style.defaultBorderSize
            color: style.listBackgroundColor
            radius: style.defaultRadius
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
                color: "white"

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
                    connectBoardPopup.close();
                    mainClient.connect_board(groupLink.text);
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
                    radius: style.defaultRadius
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
        x: root.width - 260
        y: style.headerHeight
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        focus: true
        onOpened: {
            boardTypeSwitch.checked = false;
            boardDescription.text = "";
            boardName.text = "";
        }

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: createBoardContent

            border.color: style.primaryColor
            border.width: style.defaultBorderSize
            color: style.listBackgroundColor
            radius: style.defaultRadius
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
                color: "white"

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
                color: "white"

                background: Rectangle {
                    implicitWidth: createBoardContent.width - 20
                    implicitHeight: 36
                    radius: style.defaultRadius
                    color: style.textFormColor
                    border.color: style.listBackgroundColor
                    border.width: 1
                }

            }

            Switch {
                id: boardTypeSwitch

                z: 1
                implicitWidth: parent.width - 10
                implicitHeight: 36
                anchors.top: boardDescription.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                anchors.bottomMargin: 10

                indicator: Rectangle {
                    implicitWidth: parent.width - 10
                    implicitHeight: 26
                    x: boardTypeSwitch.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 13
                    color: style.textFormColor
                    border.color: style.textFormColor

                    Rectangle {
                        x: boardTypeSwitch.checked ? parent.width - width : 0
                        width: 26
                        height: 26
                        radius: 13
                        color: "white"
                        border.color: style.primaryColor
                    }

                }

                contentItem: Text {
                    text: boardTypeSwitch.checked ? "Remote" : "Local"
                    font.family: "Poppins"
                    font.pointSize: 14
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: "white"
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
                    mainClient.create_board(boardName.text, boardDescription.text, boardTypeSwitch.checked);
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
            icon.source: "back.svg"
            icon.width: 20
            icon.height: 20

            background: Rectangle {
                color: parent.down ? Qt.darker(style.headerBackgroundColor, 1.4) : (parent.hovered ? Qt.darker(style.headerBackgroundColor, 1.2) : style.headerBackgroundColor)
            }

        }

        Text {
            text: "Welcome ^_^"
            font.family: "Poppins"
            font.pointSize: 20
            color: "white"
            anchors.centerIn: parent
        }

        Button {
            width: 136
            height: style.headerHeight / 2
            anchors.right: parent.right
            anchors.top: (mainClient && mainClient.connection_status == Client.Authorized ? parent.top : undefined)
            anchors.verticalCenter: (mainClient && mainClient.connection_status == Client.Authorized ? undefined : parent.verticalCenter)
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
            visible: mainClient ? (mainClient.connection_status == Client.Authorized) : false
            onClicked: {
                connectBoardPopup.open();
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
            model: mainClient ? mainClient.board_menu : undefined
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

                Image {
                    id: remoteImg

                    visible: is_remote
                    anchors.top: parent.top
                    anchors.topMargin: 25
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    source: "remote.svg"
                    width: 20
                    height: 20
                }

                TextInput {
                    id: nameHolder

                    width: parent.width - 60 - 40 * (is_remote)
                    text: name
                    font.family: "Poppins"
                    font.pointSize: 16
                    font.bold: true
                    color: "white"
                    maximumLength: 50
                    anchors.top: parent.top
                    anchors.left: is_remote ? remoteImg.right : parent.left
                    anchors.topMargin: 20
                    anchors.leftMargin: is_remote ? 5 : 20
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

                    width: parent.width - 40 - 40 * (is_remote)
                    height: parent.height - 110
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
                    clip: true
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

                    width: 26
                    height: 26
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.rightMargin: 20
                    anchors.topMargin: 20
                    onClicked: {
                        mainClient.delete_board(index);
                    }
                    icon.source: "bin.svg"
                    icon.width: width
                    icon.height: height

                    background: Rectangle {
                        color: parent.down ? Qt.darker(style.boardCardColor, 1.4) : (parent.hovered ? Qt.darker(style.boardCardColor, 1.2) : style.boardCardColor)
                    }

                }

                Button {
                    id: shareBoardButton

                    visible: is_remote
                    width: 26
                    height: 26
                    anchors.right: parent.right
                    anchors.top: deleteBoardButton.bottom
                    anchors.rightMargin: 20
                    anchors.topMargin: 10
                    onClicked: {
                        linkPopup.open();
                        createBoardPopup.close();
                        connectBoardPopup.close();
                    }
                    icon.source: "link.svg"
                    icon.width: 26
                    icon.height: 26

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

                Popup {
                    id: linkPopup

                    width: 240
                    height: 60
                    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
                    focus: true

                    background: Rectangle {
                        visible: false
                    }

                    contentItem: Rectangle {
                        id: linkPopUpContent

                        border.color: style.primaryColor
                        border.width: style.defaultBorderSize
                        color: style.listBackgroundColor
                        radius: style.defaultRadius
                        anchors.fill: parent
                        Drag.active: true

                        TextField {
                            id: linkHolder

                            z: 1
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.topMargin: 10
                            font.pointSize: 16
                            font.family: "Poppins"
                            readOnly: true
                            text: link
                            color: "white"

                            background: Rectangle {
                                implicitWidth: linkPopUpContent.width - 20
                                implicitHeight: 36
                                radius: style.defaultRadius
                                color: style.textFormColor
                                border.color: style.listBackgroundColor
                                border.width: 1
                            }

                        }

                        MouseArea {
                            anchors.fill: parent
                            drag.target: parent
                        }

                    }

                }

            }

        }

    }

}
