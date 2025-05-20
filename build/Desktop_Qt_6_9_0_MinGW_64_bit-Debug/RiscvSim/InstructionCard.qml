import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: card
    property string instructionName: ""
    property string instructionDescription: ""
    property bool expanded: false

    width: parent ? parent.width : 800
    height: expanded ? textBlock.implicitHeight + header.implicitHeight + 24 : header.implicitHeight + 16

    Rectangle {
        anchors.fill: parent
        radius: 12
        color: "#1e1e1e"
        border.color: "#333"
        border.width: 1

        MouseArea {
            anchors.fill: parent
            onClicked: card.expanded = !card.expanded
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            Rectangle {
                anchors.fill: parent
                color: "transparent"
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8

            Label {
                id: header
                text: instructionName
                font.pixelSize: 18
                color: "#ffffff"
                Layout.fillWidth: true
            }

            Text {
                id: textBlock
                visible: expanded
                text: instructionDescription
                wrapMode: Text.Wrap
                font.pixelSize: 14
                color: "#cccccc"
                Layout.fillWidth: true
            }
        }
    }
}
