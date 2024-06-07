import QtQuick

QtObject {
    readonly property color primaryColor: "#000000"
    readonly property color boardBackgroundColor: "#161616"
    readonly property color listBackgroundColor: "#323232"
    readonly property color boardCardColor: "#323232"
    readonly property color headerBackgroundColor: "#323232"
    readonly property color cardBackgroundColor: "#000000"
    readonly property color deleteBackgroundColor: "#ff6666"
    readonly property color createBackgroundColor: "#91C9AA"
    readonly property color defaultTextColor: "white"
    readonly property color defaultMoveColor: "#f5f5f5"
    readonly property color textFormColor: "#131313"
    readonly property int listWidth: 300
    readonly property int cardWidth: listWidth - 40
    readonly property int cardHeight: 160
    readonly property int headerHeight: 50
    readonly property int listHeaderHeight: 60
    readonly property int smallButtonSize: 30
    readonly property int defaultBorderSize: 1
    readonly property int cardPopUpWidth: 360
    readonly property int cardPopUpHeight: 240
    readonly property int defaultRadius: 10
    readonly property int cardSpacing: 10
    readonly property int moveRadius: 1
}
