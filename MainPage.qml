import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    anchors.fill: parent

    property int currentHighlightedLine: -1
    property var linePositions: []
    property bool isRunning: false

    Timer {
        id: highlightTimer
        interval: 500 // 0.5 seconds per line (adjust as needed)
        repeat: true
        onTriggered: highlightNextLine()
    }

    function sendCommandToCPU(lines) {
        var currentLineText = lines[currentHighlightedLine].trim()
        if (currentLineText.length > 0) {
            cpuWrapper.sendCommand(currentLineText)
        }
    }

    function highlightNextLine() {
        var lines = assemblyEditor.text.split('\n')

        if (lines.length === 1 && lines[0].trim() === "") {
            isRunning = false
            highlightTimer.stop()
            return
        }

        if (currentHighlightedLine + 1 >= lines.length) {
            currentHighlightedLine = -1
            isRunning = false
            highlightTimer.stop()
            return
        }

        currentHighlightedLine++

        // Scroll to line
        var pos = 0
        for (var i = 0; i < currentHighlightedLine; i++) {
            pos += lines[i].length + 1
        }
        assemblyEditor.cursorPosition = pos

        // Position highlight
        editorContainer.currentLineY = currentHighlightedLine * editorContainer.lineHeight

        sendCommandToCPU(lines)
    }

    function startExecution() {
        if (isRunning) {
            highlightTimer.stop()
        }

        var text = assemblyEditor.text
        var lines = text.split('\n')
        if (lines.length === 1 && lines[0].trim() === "") {
            highlightTimer.stop()
            return
        }
        isRunning = true
        highlightTimer.start()
    }


    function stepExecution() {
        if (!highlightTimer.running) {
            var lines = assemblyEditor.text.split('\n')
            if (lines.length === 1 && lines[0].trim() === "") {
                isRunning = false
                return
            }

            if (currentHighlightedLine === -1) {
                currentHighlightedLine = 0
                isRunning = true
            } else if (currentHighlightedLine + 1 >= lines.length) {
                consoleLog.append("End of program")
                currentHighlightedLine = -1
                isRunning = false
                return
            } else {
                currentHighlightedLine++
            }

            // Update cursor position to scroll
            var pos = 0
            for (var i = 0; i < currentHighlightedLine; i++) {
                pos += lines[i].length + 1 // newline
            }
            assemblyEditor.cursorPosition = pos

            // Move highlight
            editorContainer.currentLineY = currentHighlightedLine * editorContainer.lineHeight

            sendCommandToCPU(lines)
        }
    }


    function pauseExecution() {
        if (isRunning) {
            highlightTimer.stop()
            // isRunning = false
            consoleLog.append("Execution stopped at line " + (currentHighlightedLine + 1))
        }
    }

    function stopExecution() {
        if (isRunning) {
            highlightTimer.stop()
            isRunning = false
            currentHighlightedLine = -1
            consoleLog.append("Execution stopped at line " + (currentHighlightedLine + 1))
        }
    }

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

                        Item {
                            id: editorContainer
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            property int currentLineY: 0
                            property int lineHeight: 14 // Magic number

                            // Highlight Rectangle UNDER the text
                            Rectangle {
                                id: highlightBar
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: editorContainer.lineHeight
                                y: editorContainer.currentLineY
                                color: "#2a4d7d"
                                visible: currentHighlightedLine >= 0
                                z: 0
                            }

                            TextArea {
                                id: assemblyEditor
                                anchors.fill: parent
                                wrapMode: TextEdit.NoWrap
                                font.family: "Courier New"
                                font.pixelSize: 12
                                color: "#ffffff"
                                selectionColor: "#555555"
                                background: Rectangle { color: isRunning ? "transparent" : "#2b2b2b"; radius: 4 }
                                z: 1
                            }
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
                            onClicked: {
                                consoleLog.append("Run clicked")
                                startExecution()
                            }
                        }

                        Button {
                            text: "Step"
                            background: Rectangle { color: "#3a3f4b"; radius: 4 }
                            font.bold: true
                            font.pixelSize: 15
                            onClicked: {
                                consoleLog.append("Step clicked")
                                stepExecution()
                            }
                        }

                        Button {
                            text: "Stop"
                            background: Rectangle { color: "#3a3f4b"; radius: 4 }
                            font.bold: true
                            font.pixelSize: 15
                            onClicked: {
                                consoleLog.append("Paused execution")
                                pauseExecution()
                            }
                            onDoubleClicked: {
                                consoleLog.append("Stopped execution")
                                stopExecution()
                            }
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
