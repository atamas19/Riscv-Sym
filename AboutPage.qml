import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#121212"  // Dark background

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 32
            spacing: 24

            Label {
                text: "RISC-V Simulator"
                font.pixelSize: 32
                font.bold: true
                color: "#ffffff"
                Layout.alignment: Qt.AlignTop
            }

            Text {
                text: "This RISC-V simulator is an interactive educational tool designed to visualize the inner workings of a RISC-V CPU. You can write and run your own assembly instructions, step through execution, and observe how registers, memory, and the program counter change in real time."
                wrapMode: Text.Wrap
                font.pixelSize: 16
                color: "#cccccc"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
            }

            // Rectangle {
            //     color: "#1e1e1e"
            //     radius: 12
            //     border.color: "#333"
            //     Layout.fillWidth: true
            //     Layout.preferredHeight: 100

            //     ColumnLayout {
            //         anchors.fill: parent
            //         anchors.margins: 16
            //         spacing: 4

            //         Label {
            //             text: "Author"
            //             font.pixelSize: 14
            //             font.bold: true
            //             color: "#aaaaaa"
            //         }

            //         Label {
            //             text: "[Your Name Here]"
            //             font.pixelSize: 16
            //             color: "#ffffff"
            //         }
            //     }
            // }
        }
    }
}
