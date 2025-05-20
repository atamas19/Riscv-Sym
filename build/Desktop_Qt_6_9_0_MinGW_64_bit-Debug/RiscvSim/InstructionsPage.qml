import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#121212" // dark theme

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 32
            spacing: 16

            Label {
                text: "RISC-V Instructions"
                font.pixelSize: 32
                font.bold: true
                color: "#ffffff"
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 12
                model: ListModel {
                    ListElement {
                        name: "ADD"
                        description: "Performs addition: rd = rs1 + rs2"
                    }
                    ListElement {
                        name: "SUB"
                        description: "Performs subtraction: rd = rs1 - rs2"
                    }
                    ListElement {
                        name: "LW"
                        description: "Loads a word from memory into a register"
                    }
                    ListElement {
                        name: "SW"
                        description: "Stores a word from a register to memory"
                    }
                    ListElement {
                        name: "BEQ"
                        description: "Branch if equal: if(rs1 == rs2) jump"
                    }
                    // Add more as needed
                }

                delegate: InstructionCard {
                    instructionName: name
                    instructionDescription: description
                }
            }
        }
    }
}
