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
        height: 180
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        background: Rectangle {
            visible: false
        }

        contentItem: Rectangle {
            id: boardPopUpContent

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
                    id: groupId

                    z: 1
                    placeholderText: "Board id"
                    font.family: "Courier"
                    font.pointSize: 12
                    font.bold: true
                    Layout.preferredWidth: parent.width - 20
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                }

                Button {
                    z: 1
                    Layout.preferredWidth: parent.width - 40
                    Layout.preferredHeight: 30
                    Layout.margins: 10
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                    onClicked: {
                        connectBoardPopUp.close();
                        mainClient.connect_board(boardId.text);
                        boardId.text = "";
                    }

                    Text {
                        text: "Connect"
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

    Popup {
        id: createBoardPopup

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
                    id: boardName

                    z: 1
                    placeholderText: "New board"
                    font.family: "Courier"
                    font.pointSize: 12
                    font.bold: true
                    Layout.preferredWidth: parent.width - 20
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                }

                TextField {
                    id: boardDescription

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
                        createBoardPopup.close();
                        mainClient.create_board(boardName.text, boardDescription.text);
                        boardDescription.text = "";
                        boardName.text = "";
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
        id: boardSelectMenuBar

        visible: true
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
                onClicked: {
                    loader.active = false;
                    start_menu.visible = true;
                }

                Text {
                    text: "<"
                    font.family: "Courier"
                    font.pointSize: 12
                    anchors.centerIn: parent
                    color: "white"
                }

                background: Rectangle {
                    color: parent.down ? Qt.darker(style.primaryColor, 1.4) : (parent.hovered ? Qt.darker(style.primaryColor, 1.2) : style.primaryColor)
                }

            }

            Button {
                width: style.smallButtonSize
                height: width
                onClicked: {
                    createBoardPopup.open();
                }

                Text {
                    anchors.centerIn: parent
                    text: "+"
                    color: "white"
                }

                background: Rectangle {
                    color: parent.down ? Qt.darker(style.primaryColor, 1.4) : (parent.hovered ? Qt.darker(style.primaryColor, 1.2) : style.primaryColor)
                }

            }

            Text {
                text: (mainClient.client_mode == Client.Local) ? "local menu" : "remote menu"
                y: 3
                height: style.smallButtonSize
                font.family: "Courier"
                font.pointSize: 16
                font.bold: true
                color: "white"
            }

            Button {
                width: style.smallButtonSize
                height: width
                visible: (mainClient.client_mode == Client.Remote)
                onClicked: {
                    addGroupPopUp.open();
                }

                Text {
                    anchors.centerIn: parent
                    text: "g"
                    color: "white"
                }

                background: Rectangle {
                    color: parent.down ? Qt.darker(style.primaryColor, 1.4) : (parent.hovered ? Qt.darker(style.primaryColor, 1.2) : style.primaryColor)
                }

            }

        }

    }

    Rectangle {
        id: boardSelectMainarea

        width: root.width
        height: root.height - boardSelectMenuBar.height
        y: boardSelectMenuBar.height
        color: style.boardBackgroundColor

        ListView {
            id: boardsList

            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.topMargin: 10
            model: mainClient.board_menu
            orientation: ListView.Vertical
            spacing: 5

            delegate: Rectangle {
                id: boardCard

                width: 300
                height: 100
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter

                ColumnLayout {
                    width: parent.width
                    height: parent.height
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10

                    Text {
                        text: name
                        font.family: "Courier"
                        font.pointSize: 18
                        font.bold: true
                        Layout.preferredWidth: parent.width - 20
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: description
                        font.family: "Courier"
                        font.pointSize: 14
                        Layout.preferredWidth: parent.width - 20
                        Layout.alignment: Qt.AlignHCenter
                    }

                }

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {
                        mainClient.request_board(index);
                        loader.source = "Board.qml";
                    }
                }

            }

        }

    }

}
