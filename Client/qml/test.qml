import QtQuick
import QtQuick.Controls
import Client
import "qrc:"

Rectangle {
    id: background
    color: "#aaaaaa"
    width: root.width
    height: root.height

    MyListModel {
        id: model
    }

    Button {
        onClicked: model.add_card()
    }

    Rectangle {
        y: 20
        id: listMain  
        width: 160
        anchors.horizontalCenter: parent.horizontalCenter
        height: Math.max(90, Math.min((model.count+1) * 45,root.height-40))
        //height: root.height
        color: "#a9aaad"
        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 30  
            color: "#282c34"
            id: listheader
            Text {
                text: "Name"
                color: "white"
                anchors.centerIn: listheader
            }
            Button {
                Text {
                    anchors.centerIn: parent
                    text: "+"
                    color: "white"
                }
                width: 30
                height: 30
                background: Rectangle {
                    color: parent.down ? "#1c1e24" :
                            (parent.hovered ? "#1c1e24" : "#282c34")
                }                
                anchors.right: listheader.right
                onClicked: {
                    //listmodel.append({cardtext: "TestCard   "})
                    model.add_card()
                }
            }
        }

        Rectangle {
            id: listcontent
            width: 160
            height: listMain.height-40
            y: 30
            color: "#a9aaad"


            ListView {
                id:thislist
                clip: true
                y: 10
                anchors.fill: parent
                anchors.topMargin: 10
                anchors.leftMargin: 5
                model: model
                spacing: 5
                delegate: Card {}
            }               
        }
    }
}