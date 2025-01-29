// main.qml
import QtQuick 2.15 // Imports the QtQuick 2.15 module for UI elements
import QtQuick.Controls 2.15 // Imports the QtQuick.Controls 2.15 module for pre-built controls like buttons and text fields

ApplicationWindow { // Creates the main application window
    visible: true // Makes the window visible
    width: 400 // Sets the width of the window to 400 pixels
    height: 300 // Sets the height of the window to 300 pixels
    title: "BLE Client" // Sets the title of the window

    Column { // Creates a column layout to arrange items vertically
        anchors.centerIn: parent // Centers the column within the parent (ApplicationWindow)
        spacing: 20 // Sets the spacing between items in the column to 20 pixels

        Text { // Creates a text display element
            text: "Device Name: " + bleClient.deviceName // Binds the text to the deviceName property of the bleClient object.  This assumes you have a 'bleClient' object (likely in C++ or a QML plugin) that handles BLE communication.
            font.pixelSize: 18 // Sets the font size to 18 pixels
        }

        Text { // Creates another text display element
            text: "Status: " + bleClient.connectionStatus // Binds the text to the connectionStatus property of the bleClient object
            font.pixelSize: 18 // Sets the font size to 18 pixels
            color: bleClient.connectionStatus === "Connected" ? "green" : "red" // Sets the text color based on the connection status.  If connected, the color is green, otherwise it's red. This is a ternary operator.
        }

        Text { // Creates a text display element to show received data
            text: "Received Float: " + bleClient.receivedFloat // Binds the text to the receivedFloat property of the bleClient object.  This assumes the bleClient object receives and parses a float value.
            font.pixelSize: 18 // Sets the font size to 18 pixels
        }

        Button { // Creates a button control
            text: "Send Data" // Sets the text displayed on the button
            onClicked: { // Defines the action to be performed when the button is clicked
                bleClient.sendData("Hello ESP32"); // Calls the sendData method of the bleClient object, passing "Hello ESP32" as the data to be sent.  This assumes your bleClient object has a sendData method.
            }
        }
    }
}
