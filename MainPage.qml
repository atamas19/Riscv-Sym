import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#121212" // dark background

        RowLayout {
            anchors.fill: parent
            spacing: 16

            // === Left: Editor & Console ===
            ColumnLayout {
                spacing: 16
                Layout.preferredWidth: 600
                Layout.fillHeight: true
                Layout.leftMargin: 20
                Layout.bottomMargin: 20

                // Assembly Input
                Frame {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height * 0.5
                    Layout.topMargin: 20
                    background: Rectangle {
                        color: "#1e1e1e"
                        radius: 8
                        border.color: "#2a2a2a"
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8

                        Label {
                            text: "Assembly Input"
                            color: "white"
                            font.bold: true
                            font.pointSize: 14
                        }

                        TextArea {
                            id: assemblyEditor
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            wrapMode: TextEdit.NoWrap
                            font.family: "Courier New"
                            font.pixelSize: 12
                            color: "#ffffff"
                            selectionColor: "#555555"
                            background: Rectangle { color: "#2b2b2b"; radius: 4 }
                        }
                    }
                }

                // Controls
                Frame {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                    background: Rectangle {
                        color: "#1e1e1e"
                        radius: 8
                        border.color: "#2a2a2a"
                    }

                    RowLayout {
                        // anchors.horizontalCenter: parent.horizontalCenter
                        anchors.centerIn: parent
                        anchors.margins: 10
                        spacing: 30

                        Button {
                            text: "Run"
                            background: Rectangle { color: "#3a3f4b"; radius: 4 }
                            font.bold: true
                            font.pixelSize: 15
                            onClicked: consoleLog.append("Run clicked")
                        }

                        Button {
                            text: "Step"
                            background: Rectangle { color: "#3a3f4b"; radius: 4 }
                            font.bold: true
                            font.pixelSize: 15
                            onClicked: consoleLog.append("Step clicked")
                        }

                        Button {
                            text: "Stop"
                            background: Rectangle { color: "#3a3f4b"; radius: 4 }
                            font.bold: true
                            font.pixelSize: 15
                            onClicked: consoleLog.append("Stop clicked")
                        }
                    }
                }

                // Console Log
                Frame {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    background: Rectangle {
                        color: "#1e1e1e"
                        radius: 8
                        border.color: "#2a2a2a"
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8

                        Label {
                            text: "Console Log"
                            color: "white"
                            font.bold: true
                            font.pointSize: 14
                        }

                        TextArea {
                            id: consoleLog
                            readOnly: true
                            wrapMode: TextEdit.Wrap
                            font.pixelSize: 12
                            font.family: "Courier New"
                            color: "#ffffff"
                            selectionColor: "#555555"
                            background: Rectangle { color: "#2b2b2b"; radius: 4 }
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }
                }
            }

            // === Right: Registers & RAM ===
            ColumnLayout {
                Layout.preferredWidth: 550
                Layout.alignment: Qt.AlignTop
                Layout.bottomMargin: 20
                spacing: 16

                // Registers & PC
                Frame {
                    Layout.preferredWidth: parent.width - 20
                    Layout.topMargin: 20
                    background: Rectangle {
                        color: "#1e1e1e"
                        radius: 8
                        border.color: "#2a2a2a"
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8

                        Label {
                            text: "Registers & PC"
                            color: "white"
                            font.bold: true
                            font.pointSize: 14
                        }

                        GridLayout {
                            id: registerGrid
                            columns: 4
                            columnSpacing: 10
                            rowSpacing: 6
                            Layout.fillWidth: true

                            Repeater {
                                model: 32
                                delegate: Row {
                                    spacing: 4
                                    Label { text: "x" + index; width: 30; color: "#cccccc" }
                                    Label {
                                        text: cpuWrapper.getRegister(index)
                                        font.family: "Courier New"
                                        width: 80
                                        color: "#eeeeee"
                                    }
                                }
                            }

                            Label { text: "PC"; Layout.columnSpan: 1; color: "#cccccc" }
                            Label {
                                id: pcDisplay
                                text: "0x00000000"
                                font.family: "Courier New"
                                width: 100
                                Layout.columnSpan: 3
                                color: "#eeeeee"
                            }
                        }
                    }
                }

                // RAM Memory
                Frame {
                    Layout.preferredWidth: parent.width - 20
                    Layout.fillHeight: true
                    background: Rectangle {
                        color: "#1e1e1e"
                        radius: 8
                        border.color: "#2a2a2a"
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8

                        Label {
                            text: "RAM Memory"
                            color: "white"
                            font.bold: true
                            font.pointSize: 14
                            Layout.alignment: Qt.AlignTop
                        }

                        // ScrollView {
                        //     Layout.fillWidth: true
                        //     Layout.fillHeight: true

                        //     GridLayout {
                        //         id: ramGrid
                        //         columns: 2
                        //         columnSpacing: 10
                        //         rowSpacing: 4
                        //         Layout.fillWidth: true

                        //         Repeater {
                        //             model: 10
                        //             delegate: Row {
                        //                 spacing: 4
                        //                 Label { text: "0x" + Qt.formatNumber(index * 4, 'x', 8) + ":"; width: 80; color: "#cccccc" }
                        //                 Label {
                        //                     text: "0x00000000"
                        //                     font.family: "Courier New"
                        //                     width: 100
                        //                     color: "#eeeeee"
                        //                 }
                        //             }
                        //         }
                        //     }
                        // }
                    }
                }
            }
        }
    }
}
