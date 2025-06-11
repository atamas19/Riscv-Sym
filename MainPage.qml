import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
// import QtQuick.Controls.Material // this gets rid of a lot of errors but
// certain elements are not alligned properly and features don't work as expected.
// unless the errors will be a problem in the future, I will leave it as is

Item {
    id: root
    Layout.fillWidth: parent
    Layout.fillHeight: true

    property int currentHighlightedLine: -1
    property var linePositions: []
    property bool isRunning: false
    property var highlightedRegisters: []
    property int highlighTime: 1500
    property var highlightedMemory: []
    property var highlightTimers: ({})
    property bool stopNextStepExecution: false
    property color instructionHighlightColor: "#2a4d7d"

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

    Timer {
        id: stopExecutionTimer
        interval: highlighTime
        repeat: false
        onTriggered: {
            instructionHighlightColor = "#2a4d7d"
            stopExecution()
        }
    }

    function clearMemoryHighlights() {
        for (let i = 0; i < memoryModel.count; i++) {
            if (memoryModel.get(i).isHighlighted) {
                memoryModel.set(i, {
                    address: memoryModel.get(i).address,
                    value: memoryModel.get(i).value,
                    isHighlighted: false
                });
            }
        }
    }

    function appendToConsole(message) {
        consoleLog.text += message + "\n"
        consoleLog.cursorPosition = consoleLog.text.length
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
        pcBackground.isHighlighted = false
        highlightedRegisters = []
    }

    function clearConsoleLog() {
        consoleLog.text = ""
    }

    function sendCommandToCPU(lines) {
        clearRegisterHighlights()
        clearMemoryHighlights()
        var currentLineText = lines[currentHighlightedLine].trim()
        if (currentLineText.length > 0) {
            if (cpuWrapper.sendCommand(currentLineText) === false) {
                return false
            }
        }
        return true
    }

    function highlightAndExecuteNextLine() {
        var lines = assemblyEditor.text.split('\n')

        if (lines.length === 1 && lines[0].trim() === "") {
            isRunning = false
            highlightTimer.stop()
            cpuWrapper.resetPC()
            return
        }

        let currentPCLine = pcDisplay.currentPC / 4;
        if (currentPCLine >= lines.length || pcDisplay.currentPC % 4 !== 0) {
            stopExecution()
            return
        }

        currentHighlightedLine = currentPCLine

        var pos = 0
        for (var i = 0; i < currentHighlightedLine; i++) {
            pos += lines[i].length + 1
        }
        assemblyEditor.cursorPosition = pos

        editorContainer.currentLineY = currentHighlightedLine * editorContainer.lineHeight

        if (!sendCommandToCPU(lines)) {
            instructionHighlightColor = "#b24c4c"
            stopExecutionTimer.start()
            highlightTimer.stop()
        }
    }

    function startExecution() {
        if (isRunning) {
            return
        }

        var text = assemblyEditor.text
        var lines = text.split('\n')
        if (lines.length === 1 && lines[0].trim() === "") {
            highlightTimer.stop()
            return
        }
        isRunning = true
        highlightAndExecuteNextLine()
        if (isRunning) {
            highlightTimer.start()
        }
    }

    function stepExecution() {
        if (!highlightTimer.running) {
            var lines = assemblyEditor.text.split('\n')
            if (lines.length === 1 && lines[0].trim() === "") {
                isRunning = false
                return
            }
            if (stopNextStepExecution) {
                instructionHighlightColor = "#2a4d7d"
                stopExecution()
                return
            }

            if (currentHighlightedLine === -1) {
                currentHighlightedLine = 0
                isRunning = true
            }
            let currentPCLine = pcDisplay.currentPC / 4;
            if (currentPCLine >= lines.length || pcDisplay.currentPC % 4 !== 0) {
                stopExecution()
                return
            }

            currentHighlightedLine = currentPCLine

            var pos = 0
            for (var i = 0; i < currentHighlightedLine; i++) {
                pos += lines[i].length + 1
            }
            assemblyEditor.cursorPosition = pos

            editorContainer.currentLineY = currentHighlightedLine * editorContainer.lineHeight

            if (!sendCommandToCPU(lines)) {
                instructionHighlightColor = "#b24c4c"
                stopNextStepExecution = true
            }
        }
    }

    function pauseExecution() {
        if (isRunning) {
            highlightTimer.stop()
        }
    }

    function stopExecution() {
        stopNextStepExecution = false
        highlightTimer.stop()
        isRunning = false
        currentHighlightedLine = -1
        cpuWrapper.resetPC()
        clearRegisterHighlights()
        clearMemoryHighlights()
    }

    function reset() {
        stopNextStepExecution = false
        highlightTimer.stop()
        isRunning = false
        currentHighlightedLine = -1
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
                                color: instructionHighlightColor
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

                            ScrollBar.vertical: ScrollBar {
                                policy: ScrollBar.AlwaysOff
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

                            RowLayout {
                                Layout.columnSpan: 4
                                spacing: 19 // magic number

                                Label {
                                    text: "PC"
                                    color: "#cccccc"
                                    font.bold: true
                                    Layout.alignment: Qt.AlignVCenter
                                }

                                Rectangle {
                                    id: pcBackground
                                    width: 80
                                    height: 20
                                    radius: 4

                                    property bool isHighlighted: false

                                    color: isHighlighted ? "#2a4d7d" : "#2b2b2b"

                                    Behavior on color {
                                        ColorAnimation {
                                            duration: 400
                                            easing.type: Easing.InOutQuad
                                        }
                                    }

                                    Label {
                                        id: pcDisplay
                                        anchors.fill: parent
                                        property int currentPC: 0
                                        text: currentPC
                                        font.family: "Courier New"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        color: "#eeeeee"
                                    }

                                    Connections {
                                        target: cpuWrapper
                                        function onPcChanged(newPc) {
                                            pcDisplay.currentPC = newPc
                                            pcBackground.isHighlighted = true
                                        }
                                    }
                                }
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

                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            ListView {
                                id: memoryView
                                model: memoryModel
                                cacheBuffer: 100
                                delegate: Rectangle {
                                    width: memoryView.width
                                    height: 25
                                    color: model.isHighlighted ? "#2a4d7d" : "transparent"
                                    radius: 4

                                    Behavior on color {
                                        ColorAnimation {
                                            duration: 300
                                            easing.type: Easing.InOutQuad
                                        }
                                    }

                                    Row {
                                        anchors.fill: parent
                                        spacing: 16
                                        anchors.margins: 4

                                        Label {
                                            text: "0x" + address.toString(16).padStart(8, "0").toUpperCase() + ":"
                                            color: "#cccccc"
                                            font.family: "Courier New"
                                            width: 120
                                        }

                                        Label {
                                            text: "0x" + value.toString(16).padStart(8, "0").toUpperCase()
                                            color: "#eeeeee"
                                            font.family: "Courier New"
                                            width: 120
                                        }

                                        Label {
                                            text: {
                                                const bytes = [
                                                    (value >> 24) & 0xFF,
                                                    (value >> 16) & 0xFF,
                                                    (value >> 8) & 0xFF,
                                                    value & 0xFF
                                                ];
                                                return bytes.map(b => b.toString(16).padStart(2, "0").toUpperCase()).join(" ");
                                            }
                                            color: "#aaaaaa"
                                            font.family: "Courier New"
                                            width: 120
                                        }

                                        Label {
                                            text: {
                                                const bytes = [
                                                    (value >> 24) & 0xFF,
                                                    (value >> 16) & 0xFF,
                                                    (value >> 8) & 0xFF,
                                                    value & 0xFF
                                                ];
                                                return bytes.map(b => (b >= 32 && b <= 126) ? String.fromCharCode(b) : ".").join("");
                                            }
                                            color: "#ffffff"
                                            font.family: "Courier New"
                                            width: 80
                                        }
                                    }
                                }

                                clip: true

                                onCountChanged: {
                                    if (count > 0) {
                                        Qt.callLater(() => {
                                            memoryView.positionViewAtIndex(count - 1, ListView.End);
                                        });
                                    }
                                }
                            }

                            ScrollBar.vertical: ScrollBar {
                                policy: ScrollBar.AlwaysOff
                            }


                            ListModel {
                                id: memoryModel
                            }
                            Connections {
                                target: cpuWrapper
                                function onAddMemoryEntry(address, value) {
                                    for (let i = 0; i < memoryModel.count; i++) {
                                        if (memoryModel.get(i).address === address) {
                                            memoryModel.set(i, {
                                                address: address,
                                                value: value,
                                                isHighlighted: true
                                            });
                                            return;
                                        }
                                    }

                                    memoryModel.append({
                                        address: address,
                                        value: value,
                                        isHighlighted: true
                                    });
                                }

                                function onClearMemory() {
                                    memoryModel.clear()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
