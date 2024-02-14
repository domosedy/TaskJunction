import QtQuick
import QtQuick.Controls
import "qrc:"


Rectangle {

    id: main
    visible: true

    Rectangle {
        id: topmenu
        visible:true
        width: root.width
        height: 30
        color: "#282c34"
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
                color: parent.down ? "#1c1e24" :
                        (parent.hovered ? "#1c1e24" : "#282c34")
            }
            onClicked: {
                boardmodel.append({name: "test"})
            }
        }
    }
    Rectangle {
        id: mainarea
        width: root.width
        height: root.height-topmenu.height
        y: topmenu.height
        color: "#686b70"

        ListModel {
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

