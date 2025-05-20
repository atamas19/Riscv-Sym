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
                                        mainLoader.source = "MainPage.qml"
                                        break
                                    case 1:
                                        mainLoader.source = "InstructionsPage.qml"
                                        break
                                    case 2:
                                        mainLoader.source = "AboutPage.qml"
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

                    // Label {
                    //     text: "v0.0.1"
                    //     color: "#aaaaaa"
                    //     font.pixelSize: 12
                    //     horizontalAlignment: Text.AlignHCenter
                    //     Layout.alignment: Qt.AlignHCenter
                    // }
                }
            }

            // Main Content Area
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0

                // Rectangle {
                //     height: 60
                //     color: "#ffffff"
                //     Layout.fillWidth: true

                //     Text {
                //         anchors.centerIn: parent
                //         text: "Dashboard"
                //         font.pixelSize: 20
                //         color: "#333"
                //     }
                // }

                StackLayout {
                    id: mainContentArea
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // Loader { source: "WelcomePage.qml" } // I will implement this later, for now I have other priorities
                    Loader {
                        id: mainLoader
                        source: "MainPage.qml"
                    }

                    // Rectangle {
                    //     color: "red";
                    //     Layout.alignment: AlignCenter
                    //     Text { anchors.centerIn: parent; text: "Home" }
                    // }
                    // Rectangle { color: "#fefefe"; Text { anchors.centerIn: parent; text: "Documents" } }
                    // Rectangle { color: "#ffffff"; Text { anchors.centerIn: parent; text: "Settings" } }
                    // Rectangle { color: "#fefefe"; Text { anchors.centerIn: parent; text: "Help" } }
                }
            }
        }
    }
}
