import QtQuick
import QtQuick.Controls
import Client
import "qrc:"

Component {
    Rectangle {
        id: listMain 
        signal deleteRequest(int index)
        y: 20 
        width: style.listWidth
        height: Math.max(90, Math.min((listmodel.count+1) * 45,mainarea.height-40))
        color: style.listBackgroundColor
        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: style.headerHeight 
            color: style.primaryColor
            id: listheader
            Text {
                text: name
                color: "white"
                font.family: "Courier"
                font.pointSize: 16
                anchors.centerIn: listheader
            }
            Button {
                Text {
                    anchors.centerIn: parent
                    text: "+"
                    color: "white"
                }
                height: listheader.height
                width: height
                background: Rectangle {
                    color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                            (parent.hovered ? Qt.darker(style.primaryColor, 1.4) : style.primaryColor)
                }            
                anchors.left: listheader.left
                onClicked: {
                    listmodel.add_card()
                }
            }
            Button {
                Text {
                    anchors.centerIn: parent
                    text: "-"
                    color: "white"
                }
                height: listheader.height
                width: height
                background: Rectangle {
                    color: parent.down ? Qt.darker(style.primaryColor, 1.4) :
                            (parent.hovered ? Qt.darker(style.primaryColor, 1.4) : style.primaryColor)
                }            
                anchors.right: listheader.right
                onClicked: deleteRequest(index)
            }            
        }

        Rectangle {
            id: listcontent
            width: parent.width
            height: listMain.height-40
            anchors.top: listheader.bottom
            color: style.listBackgroundColor

            ListView {
                id:thislist
                clip: true
                y: 10
                anchors.fill: parent
                anchors.topMargin: 10
                anchors.leftMargin: 5
                model: listmodel
                spacing: 5
                delegate: Card {}               
            }             
        }
        Component.onCompleted: {
            deleteRequest.connect(listview.model.delete_list);
        }
    }
}