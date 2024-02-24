import QtQuick
import QtQuick.Controls
import Client
import "qrc:"


Rectangle {

    id: main
    visible: true

    Rectangle {
        id: topmenu
        visible:true
        width: root.width
        height: style.headerHeight
        color: style.primaryColor
        anchors.top: root.top
        z: 100

        Button {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            height: topmenu.height
            width: height
            Text {
                anchors.centerIn: parent
                text: "+"
                color: "white"
            }
            background: Rectangle {
                color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                        (parent.hovered ? Qt.darker(style.primaryColor, 1.4) : style.primaryColor)
            }
            onClicked: {
                boardmodel.add_list()
            }
        }
    }
    Rectangle {
        id: mainarea
        width: root.width
        height: root.height-topmenu.height
        y: topmenu.height
        color: style.boardBackgroundColor

        ClientBoardModel {
            id: boardmodel
        }

        ListView {
            id: listview
            anchors.fill: parent
            anchors.leftMargin: 10
            model: boardmodel
            orientation: ListView.Horizontal
            spacing: 5
            delegate: List {}
        }
    }
}

