import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: card
    property string instructionName: ""
    property string instructionFullName: ""
    property string instructionFormat: ""
    property string instructionDescription: ""
    property bool expanded: false

    width: parent ? parent.width : 800
    height: expanded
        ? header.implicitHeight +
          fullNameLabel.implicitHeight +
          formatLabel.implicitHeight +
          descriptionText.implicitHeight +
          48
        : header.implicitHeight + 20

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
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 6

            Label {
                id: header
                text: instructionName
                font.pixelSize: 20
                font.bold: true
                color: "#ffffff"
                Layout.fillWidth: true
            }

            Label {
                id: fullNameLabel
                visible: expanded
                text: instructionFullName
                font.bold: true
                font.pixelSize: 17
                color: "#bbbbbb"
                Layout.fillWidth: true
            }

            Label {
                id: formatLabel
                visible: expanded
                text: "Assembly format: "
                font.pixelSize: 17
                color: "#cccccc"
                Label {
                    id: formatLabelAsmFormat
                    x: formatLabel.width
                    visible: expanded
                    text: instructionFormat
                    font.pixelSize: 15
                    font.family: "monospace"
                    color: "#e0e0e0"
                    padding: 6
                    background: Rectangle {
                        color: "#2d2d2d"
                        radius: 6
                        border.color: "#555"
                        border.width: 1
                    }
                }
            }

            Text {
                id: descriptionText
                visible: expanded
                text: instructionDescription
                wrapMode: Text.Wrap
                font.pixelSize: 17
                color: "#cccccc"
                Layout.fillWidth: true
            }
        }
    }
}
