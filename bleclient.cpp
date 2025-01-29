#include "bleclient.h"
#include <QDebug>

// Constructor implementation
BLEClient::BLEClient(QObject *parent) : QObject(parent)
{
    // Initialize default values
    m_deviceName = "Not Connected";
    m_connectionStatus = "Disconnected";
}

// Method to connect to a BLE device
void BLEClient::connectToDevice(const QBluetoothDeviceInfo &deviceInfo)
{
    // Create a BLE controller for the device
    m_controller = QLowEnergyController::createCentral(deviceInfo, this);
    
    // Connect signals to slots for handling BLE events
    connect(m_controller, &QLowEnergyController::connected, this, &BLEClient::deviceConnected);
    connect(m_controller, &QLowEnergyController::disconnected, this, &BLEClient::deviceDisconnected);
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &BLEClient::serviceDiscovered);
    connect(m_controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::errorOccurred), this, &BLEClient::errorReceived);
    
    // Attempt to connect to the device
    m_controller->connectToDevice();
}

// Getter for the device name
QString BLEClient::deviceName() const
{
    return m_deviceName;
}

// Getter for the connection status
QString BLEClient::connectionStatus() const
{
    return m_connectionStatus;
}

// Slot called when the device is connected
void BLEClient::deviceConnected()
{
    qDebug() << "device connected";
    m_connectionStatus = "Connected";
    m_deviceName = m_controller->remoteName(); // Get the name of the connected device
    emit deviceNameChanged(); // Notify that the device name has changed
    emit connectionStatusChanged(); // Notify that the connection status has changed
    qDebug() << "Device connected, discovering services...";
    m_controller->discoverServices(); // Start discovering services on the device
}

// Slot called when the device is disconnected
void BLEClient::deviceDisconnected()
{
    m_connectionStatus = "Disconnected";
    emit connectionStatusChanged(); // Notify that the connection status has changed
    qDebug() << "Device disconnected";
}

// Slot called when a BLE service is discovered
void BLEClient::serviceDiscovered(const QBluetoothUuid &serviceUuid)
{
    qDebug() << "Service discovered:" << serviceUuid.toString();
    
    // Check if the discovered service matches the ESP32 service UUID
    if (serviceUuid == QBluetoothUuid(QUuid(SERVICE_UUID))) {
        qDebug() << "ESP32 service found, creating QLowEnergyService...";
        m_service = m_controller->createServiceObject(serviceUuid, this); // Create a service object
        
        if (m_service) {
            // Connect service signals to slots
            connect(m_service, &QLowEnergyService::stateChanged, this, &BLEClient::serviceStateChanged);
            connect(m_service, &QLowEnergyService::characteristicChanged, this, &BLEClient::characteristicChanged);
            m_service->discoverDetails(); // Discover service details
        } else {
            qWarning() << "Failed to create QLowEnergyService for service:" << serviceUuid.toString();
        }
    }
}

// Slot called when an error occurs
void BLEClient::errorReceived(QLowEnergyController::Error error)
{
    qWarning() << "Error:" << error;
    m_connectionStatus = "Error" + QString::number(error);
    emit connectionStatusChanged(); // Notify that the connection status has changed
}

// Slot called when a characteristic value changes
void BLEClient::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    qDebug() << "Characteristic changed:" << characteristic.uuid().toString();
    
    // Check if the changed characteristic is the TX characteristic
    if (characteristic.uuid() == QBluetoothUuid(QUuid(CHARACTERISTIC_UUID_TX))) {
        memcpy(&m_receivedFloat, newValue.constData(), sizeof(float)); // Copy the received float value
        qDebug() << "Received value:" << m_receivedFloat;
        emit floatReceived(m_receivedFloat); // Notify that a new float value has been received
    }
}

// Getter for the received float value
float BLEClient::receivedFloat() const
{
    return m_receivedFloat;
}

// Method to send data to the BLE device
void BLEClient::sendData(const QByteArray &data) {
    // Check if the service and RX characteristic are valid
    if (m_service && m_service->state() == QLowEnergyService::RemoteServiceDiscovered && m_rxCharacteristic.isValid()) {
        m_service->writeCharacteristic(m_rxCharacteristic, data); // Write data to the RX characteristic
        qDebug() << "Data sent:" << data;
    } else {
        // Log warnings if conditions are not met
        if (!m_service) {
            qWarning() << "Service is not initialized";
        } else if (m_service->state() != QLowEnergyService::RemoteServiceDiscovered) {
            qWarning() << "Service is not in RemoteServiceDiscovered state";
        } else if (!m_rxCharacteristic.isValid()) {
            qWarning() << "RX Characteristic is not valid";
        }
    }
}

// Slot called when the service state changes
void BLEClient::serviceStateChanged(QLowEnergyService::ServiceState newState) {
    if (newState == QLowEnergyService::RemoteServiceDiscovered) {
        qDebug() << "Service details discovered";
        
        // Log all characteristics of the service
        QList<QLowEnergyCharacteristic> characteristics = m_service->characteristics();
        for (const QLowEnergyCharacteristic &characteristic : characteristics) {
            qDebug() << "Characteristic found: " << characteristic.uuid().toString();
        }
        
        // Get the TX characteristic
        QLowEnergyCharacteristic txCharacteristic = m_service->characteristic(QBluetoothUuid(QUuid(CHARACTERISTIC_UUID_TX)));
        if (txCharacteristic.isValid()) {
            qDebug() << "TX Characteristic found, enabling notifications...";
            QLowEnergyDescriptor notification = txCharacteristic.descriptor(QBluetoothUuid(QUuid("00002902-0000-1000-8000-00805f9b34fb")));
            if (notification.isValid()) {
                m_service->writeDescriptor(notification, QByteArray::fromHex("0100")); // Enable notifications
                qDebug() << "Notifications enabled for TX characteristic:" << txCharacteristic.uuid().toString();
            } else {
                qWarning() << "Invalid notification descriptor for TX characteristic:" << txCharacteristic.uuid().toString();
            }
        } else {
            qWarning() << "TX Characteristic not found:" << CHARACTERISTIC_UUID_TX;
        }
        
        // Get the RX characteristic
        m_rxCharacteristic = m_service->characteristic(QBluetoothUuid(QUuid(CHARACTERISTIC_UUID_RX)));
        if (m_rxCharacteristic.isValid()) {
            qDebug() << "RX Characteristic found:" << m_rxCharacteristic.uuid().toString();
        } else {
            qWarning() << "RX Characteristic not found:" << CHARACTERISTIC_UUID_RX;
        }
    }
}
