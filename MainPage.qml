import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
// import QtQuick.Controls.Material // this gets rid of a lot of errors but
// certain elements are not alligned properly and features don't work as expected.
// unless the errors will be a problem in the future, I will leave it as is

Item {
    id: root
    anchors.fill: parent

    property int currentHighlightedLine: -1
    property var linePositions: []
    property bool isRunning: false
    property var highlightedRegisters: []
    property int highlighTime: 1500

    Timer {
        id: highlightTimer
        interval: highlighTime
        repeat: true
        onTriggered: highlightAndExecuteNextLine()
    }

    Timer {
        id: highlightClearTimer
        interval: highlighTime
        repeat: false
        onTriggered: clearRegisterHighlights()
    }

    function appendToConsole(message) {
        consoleLog.text = message + "\n" + consoleLog.text // Prepend new messages
    }

    function clearRegisterHighlights() {
        for (let i = 0; i < highlightedRegisters.length; i++) {
            let index = highlightedRegisters[i]
            let item = registerRepeater.itemAt(index);
            if (item && item.regBackgroundRect) {
                item.regBackgroundRect.isHighlighted = false;
            } else {
                appendToConsole("No item or regBackground at index " + index)
            }
        }
        highlightedRegisters = []
    }

    function clearConsoleLog() {
        consoleLog.text = ""
    }

    function sendCommandToCPU(lines) {
        clearRegisterHighlights()
        var currentLineText = lines[currentHighlightedLine].trim()
        if (currentLineText.length > 0) {
            cpuWrapper.sendCommand(currentLineText)
        }
    }

    function highlightAndExecuteNextLine() {
        var lines = assemblyEditor.text.split('\n')

        if (lines.length === 1 && lines[0].trim() === "") {
            isRunning = false
            highlightTimer.stop()
            return
        }

        if (currentHighlightedLine + 1 >= lines.length) {
            currentHighlightedLine = -1
            isRunning = false
            clearRegisterHighlights()
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
        highlightAndExecuteNextLine()
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
                appendToConsole("End of program")
                currentHighlightedLine = -1
                isRunning = false
                clearRegisterHighlights()
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
            appendToConsole("Execution stopped at line " + (currentHighlightedLine + 1))
        }
    }

    function stopExecution() {
        if (isRunning) {
            highlightTimer.stop()
            isRunning = false
            currentHighlightedLine = -1
            appendToConsole("Execution stopped at line " + (currentHighlightedLine + 1))
            clearRegisterHighlights()
        }
    }

    function reset() {
        cpuWrapper.reset()
        clearConsoleLog()
        highlightClearTimer.start()
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
                                background: Rectangle {
                                    color: isRunning ? "transparent" : "#2b2b2b"
                                    radius: 4
                                }
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
                                startExecution()
                            }
                        }

                        Button {
                            text: "Step"
                            background: Rectangle { color: "#3a3f4b"; radius: 4 }
                            font.bold: true
                            font.pixelSize: 15
                            onClicked: {
                                stepExecution()
                            }
                        }

                        Button {
                            text: "Stop"
                            background: Rectangle { color: "#3a3f4b"; radius: 4 }
                            font.bold: true
                            font.pixelSize: 15
                            onClicked: {
                                pauseExecution()
                            }
                            onDoubleClicked: {
                                stopExecution()
                            }
                        }
                        Button {
                            text: "Reset"
                            background: Rectangle { color: "#3a3f4b"; radius: 4 }
                            font.bold: true
                            font.pixelSize: 15
                            onClicked: {
                                reset()
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
                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            ScrollBar.vertical: ScrollBar { // TODO: fix the scrollbar not showing
                                policy: ScrollBar.AlwaysOn
                                width: 6
                                contentItem: Rectangle {
                                    implicitWidth: 6
                                    radius: 3
                                    color: "#555"
                                }
                                background: Rectangle {
                                    color: "#2b2b2b"
                                }
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

                                Connections {
                                    target: cpuWrapper
                                    function onLogMessage(message) {
                                        appendToConsole(message)
                                    }
                                }
                            }
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
                                id: registerRepeater
                                model: 32
                                delegate: Row { // TODO: Change the layout so the registers are ordered vertically not horizontally
                                    spacing: 4
                                    property Rectangle regBackgroundRect: regBackground

                                    Label { text: "x" + index; width: 30; color: "#cccccc" }
                                    Rectangle {
                                        id: regBackground
                                        width: 80
                                        height: 20
                                        radius: 4

                                        property bool isHighlighted: false

                                        color: isHighlighted ? "#2a4d7d" : "#2b2b2b"  // highlighted vs normal bg colors

                                        Behavior on color {
                                            ColorAnimation {
                                                duration: 400
                                                easing.type: Easing.InOutQuad
                                            }
                                        }

                                        Label {
                                            id: regLabel
                                            anchors.fill: parent
                                            text: cpuWrapper ? cpuWrapper.getRegister(index) : "N/A"
                                            font.family: "Courier New"
                                            color: "#eeeeee"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                        Connections {
                                            target: cpuWrapper
                                            function onRegistersChanged(regIndices) {
                                                for (let i = 0; i < regIndices.length; i++) {
                                                    let idx = regIndices[i]
                                                    let item = registerRepeater.itemAt(idx)
                                                    if (item && item.children[1]) {
                                                        item.children[1].isHighlighted = true
                                                        item.children[1].children[0].text = cpuWrapper.getRegister(idx)
                                                        highlightedRegisters.push(idx)
                                                    }
                                                }
                                            }
                                        }
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
