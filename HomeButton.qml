import QtQuick

Item {
    id: root
    property alias text: textID
    signal clicked()

    width: 120
    height: 40

    Rectangle {
        id: background
        anchors.fill: parent
        radius: 8
        color: hovered ? "#87cefa" : "#4682b4"
        border.color: "#2c3e50"
        border.width: 1

        property bool hovered: false

        Text {
            id: textID
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 16
            font.bold: true
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: background.hovered = true
            onExited: background.hovered = false
            onClicked: console.log("Button" + textID.text + " clicked!")
        }
    }
}

// Item {
//     width: 400
//     height: 300

//     property alias textId: textId

//     Rectangle {
//         id: customButton
//         width: 120
//         height: 40
//         color: hovered ? "#87cefa" : "#4682b4"
//         radius: 8
//         anchors.centerIn: parent
//         border.color: "#2c3e50"
//         border.width: 1

//         property bool hovered: false

//         Text {
//             id: textId
//             anchors.centerIn: parent
//             text: "Custom Button"
//             color: "white"
//             font.pixelSize: 16
//             font.bold: true
//         }

//         MouseArea {
//             anchors.fill: parent
//             hoverEnabled: true
//             onEntered: customButton.hovered = true
//             onExited: customButton.hovered = false
//             onClicked: {
//                 console.log("Custom button clicked!")
//             }
//         }
//     }
// }
