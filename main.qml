import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300
    title: "BLE Client"

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Device Name: " + bleClient.deviceName
            font.pixelSize: 18
        }

        Text {
            text: "Status: " + bleClient.connectionStatus
            font.pixelSize: 18
            color: bleClient.connectionStatus === "Connected" ? "green" : "red"
        }

        Text {
            // text: "Received Value: " + bleClient.receivedValue
            text: "Received Message: " + bleClient.receivedMessage
            font.pixelSize: 18
        }
    }
}
