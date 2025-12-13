import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    Layout.fillWidth: parent
    Layout.fillHeight: true

    Rectangle {
        anchors.fill: parent
        color: "#121212"

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
                text: "This RISC-V simulator is an interactive educational tool designed to visualize the inner workings of a RISC-V CPU. It allows you to write and execute RV32I assembly code — the base integer subset of the RISC-V instruction set — step through instructions one at a time, and observe in real time how the CPU registers, memory, and program counter evolve.\n\nWhether you're a student learning about computer architecture or simply curious about how low-level code interacts with hardware, this simulator offers a hands-on environment to experiment and learn.\n\nTo get started, visit the 'Instructions' page, where you’ll find a detailed list of all supported RV32I instructions. Each entry includes a description, the instruction format, and an explanation of what the instruction does, helping you better understand how to write valid and meaningful assembly code.\n\nThen, head over to the 'Home' page, where you can enter your own assembly programs in the built-in editor. You can execute your code fully or one instruction at a time using the available controls. As you run your assembly code, the simulator will visually update the state of all CPU registers and memory cells, and will also log detailed messages in the console to show exactly what happens during execution. This helps reinforce your understanding of how instructions are decoded and executed, and how they affect the machine's internal state."
                wrapMode: Text.Wrap
                font.pixelSize: 16
                color: "#cccccc"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
            }
        }
    }
}
