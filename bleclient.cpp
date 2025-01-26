#include "bleclient.h"
#include <QDebug>

BLEClient::BLEClient(QObject *parent) : QObject(parent)
{
    m_deviceName = "Not Connected";
    m_connectionStatus = "Disconnected";
}

void BLEClient::connectToDevice(const QBluetoothDeviceInfo &deviceInfo)
{
    m_controller = QLowEnergyController::createCentral(deviceInfo, this);
    connect(m_controller, &QLowEnergyController::connected, this, &BLEClient::deviceConnected);
    connect(m_controller, &QLowEnergyController::disconnected, this, &BLEClient::deviceDisconnected);
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &BLEClient::serviceDiscovered);
    connect(m_controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::errorOccurred), this, &BLEClient::errorReceived);

    m_controller->connectToDevice();
}

QString BLEClient::deviceName() const
{
    return m_deviceName;
}

QString BLEClient::connectionStatus() const
{
    return m_connectionStatus;
}

void BLEClient::deviceConnected()
{
    qDebug() << "device connected";
    m_connectionStatus = "Connected";
    m_deviceName = m_controller->remoteName();
    emit deviceNameChanged();
    emit connectionStatusChanged();
    qDebug() << "Device connected, discovering services...";
    m_controller->discoverServices();
}

void BLEClient::deviceDisconnected()
{
    m_connectionStatus = "Disconnected";
    emit connectionStatusChanged();
    qDebug() << "Device disconnected";
}

void BLEClient::serviceDiscovered(const QBluetoothUuid &serviceUuid)
{
    qDebug() << "Service discovered:" << serviceUuid.toString();

    // Check if the discovered service matches the ESP32 service UUID
    if (serviceUuid == QBluetoothUuid(QUuid(SERVICE_UUID))) {
        qDebug() << "ESP32 service found, creating QLowEnergyService...";
        m_service = m_controller->createServiceObject(serviceUuid, this);

        if (m_service) {
            connect(m_service, &QLowEnergyService::stateChanged, this, &BLEClient::serviceStateChanged);
            connect(m_service, &QLowEnergyService::characteristicChanged, this, &BLEClient::characteristicChanged);
            m_service->discoverDetails();
        } else {
            qWarning() << "Failed to create QLowEnergyService for service:" << serviceUuid.toString();
        }
    }
}

void BLEClient::errorReceived(QLowEnergyController::Error error)
{
    qWarning() << "Error:" << error;
    m_connectionStatus = "Error" + QString::number(error);
    emit connectionStatusChanged();
}

void BLEClient::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    qDebug() << "Characteristic changed:" << characteristic.uuid().toString();

    if (characteristic.uuid() == QBluetoothUuid(QUuid(CHARACTERISTIC_UUID_TX))) {
        memcpy(&m_receivedFloat, newValue.constData(), sizeof(float));
        qDebug() << "Received value:" << m_receivedFloat;
        emit floatReceived(m_receivedFloat);
    }
}

float BLEClient::receivedFloat() const
{
    return m_receivedFloat;
}

void BLEClient::sendData(const QByteArray &data) {
    if (m_service && m_rxCharacteristic.isValid()) {
        m_service->writeCharacteristic(m_rxCharacteristic, data);
    } else {
        qWarning() << "Service or characteristic not valid for sending data";
    }
}

void BLEClient::serviceStateChanged(QLowEnergyService::ServiceState newState) {
    if (newState == QLowEnergyService::RemoteServiceDiscovered) {
        qDebug() << "Service details discovered";

        // Get the TX characteristic
        QLowEnergyCharacteristic txCharacteristic = m_service->characteristic(QBluetoothUuid(QUuid(CHARACTERISTIC_UUID_TX)));
        if (txCharacteristic.isValid()) {
            qDebug() << "TX Characteristic found, enabling notifications...";
            QLowEnergyDescriptor notification = txCharacteristic.descriptor(QBluetoothUuid(QUuid("00002902-0000-1000-8000-00805f9b34fb")));
            if (notification.isValid()) {
                m_service->writeDescriptor(notification, QByteArray::fromHex("0100"));
                qDebug() << "Notifications enabled for TX characteristic:" << txCharacteristic.uuid().toString();
            } else {
                qWarning() << "Invalid notification descriptor for TX characteristic:" << txCharacteristic.uuid().toString();
            }
        } else {
            qWarning() << "TX Characteristic not found:" << CHARACTERISTIC_UUID_TX;
        }

        // Get the RX characteristic
        m_rxCharacteristic = m_service->characteristic(QBluetoothUuid(QUuid(CHARACTERISTIC_UUID_RX)));
        if (!m_rxCharacteristic.isValid()) {
            qWarning() << "RX Characteristic not found:" << CHARACTERISTIC_UUID_RX;
        }
    }
}
