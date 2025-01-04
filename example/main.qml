// main.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    anchors.fill: parent
    color: "lightblue"

    Text {
        text: "Hello from QML!"
        anchors.centerIn: parent
        font.pixelSize: 24
    }

    TextField {
        id: inputBox
        width: 200
        placeholderText: "Enter text here"
        font.pixelSize: 16
        inputMethodHints: Qt.ImhPreferLowercase | Qt.ImhNoPredictiveText
    }
}

