#ifndef BLECLIENT_H
#define BLECLIENT_H

// Include necessary Qt headers for BLE functionality
#include <QObject>
#include <QLowEnergyController>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QLowEnergyService>

// Define UUIDs for the BLE service and characteristics
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e" // UUID for RX characteristic (receiving data)
#define CHARACTERISTIC_UUID_TX "beb5483e-36e1-4688-b7f5-ea07361b26a8" // UUID for TX characteristic (transmitting data)
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"          // UUID for the BLE service

// Define the BLEClient class, which handles BLE communication
class BLEClient : public QObject
{
    Q_OBJECT
    // Declare properties for QML binding
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)         // Property for the device name
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged) // Property for connection status
    Q_PROPERTY(float receivedFloat READ receivedFloat NOTIFY floatReceived)         // Property for received float data

public:
    explicit BLEClient(QObject *parent = nullptr); // Constructor
    void connectToDevice(const QBluetoothDeviceInfo &deviceInfo); // Method to connect to a BLE device
    Q_INVOKABLE void sendData(const QByteArray &data); // Method to send data to the BLE device (callable from QML)

    // Getter methods for properties
    QString deviceName() const;
    QString connectionStatus() const;
    float receivedFloat() const;

signals:
    // Signals to notify property changes
    void deviceNameChanged();
    void connectionStatusChanged();
    void floatReceived(float value); // Signal emitted when a float value is received

private slots:
    // Slot methods to handle BLE events
    void deviceConnected(); // Called when the device is connected
    void deviceDisconnected(); // Called when the device is disconnected
    void serviceDiscovered(const QBluetoothUuid &serviceUuid); // Called when a BLE service is discovered
    void serviceStateChanged(QLowEnergyService::ServiceState newState); // Called when the service state changes
    void errorReceived(QLowEnergyController::Error error); // Called when an error occurs
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue); // Called when a characteristic value changes

private:
    // Private member variables
    QLowEnergyController *m_controller = nullptr; // BLE controller for managing the connection
    QLowEnergyService *m_service = nullptr;       // BLE service object
    QString m_deviceName;                         // Stores the name of the connected device
    QString m_connectionStatus;                   // Stores the current connection status
    float m_receivedFloat = 0.0f;                 // Stores the received float value
    QLowEnergyCharacteristic m_rxCharacteristic;  // Characteristic for receiving data
};

#endif // BLECLIENT_H
