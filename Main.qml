import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Effects

Window {
    visible: true
    visibility: "Maximized"
    width: 1280
    height: 720
    title: "RISC-V CPU Simulator"
    color: "#f0f0f0"

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"

        RowLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                id: sidebar
                width: 220
                color: "#1e1e2f"
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    Label {
                        text: "RISC-V SIM"
                        font.pixelSize: 24
                        color: "#ffffff"
                        font.bold: true
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Repeater {
                        model: ListModel {
                            ListElement { icon: "🏠"; label: "Home" }
                            ListElement { icon: "📄"; label: "Instructions" }
                            ListElement { icon: "❓"; label: "About" }
                        }

                        delegate: Item {
                            width: parent.width
                            height: 50

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    switch (index) {
                                    case 0:
                                        mainContentArea.currentIndex = 0
                                        break
                                    case 1:
                                        mainContentArea.currentIndex = 1
                                        break
                                    case 2:
                                        mainContentArea.currentIndex = 2
                                        break
                                    }
                                }

                                property bool hovered: false
                                onEntered: hovered = true
                                onExited: hovered = false

                                Rectangle {
                                    anchors.fill: parent
                                    color: parent.hovered ? "#2d2d44" : "transparent"
                                    radius: 10

                                    Row {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        spacing: 10

                                        Label {
                                            text: model.icon
                                            font.pixelSize: 20
                                            color: "#ffffff"
                                        }
                                        Label {
                                            text: model.label
                                            color: "#ffffff"
                                            font.pixelSize: 16
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Rectangle {
                        Layout.fillHeight: true
                        color: "transparent"
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0

                StackLayout {
                    id: mainContentArea
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: 0

                    MainPage {
                        id: mainPage
                    }

                    InstructionsPage {
                        id: instructionsPage
                    }

                    AboutPage {
                        id: aboutPage
                    }
                }
            }
        }
    }
}
