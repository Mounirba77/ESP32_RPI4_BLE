#include "BLEClient.h"
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

void BLEClient::serviceStateChanged(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::RemoteServiceDiscovered) {
        qDebug() << "Service details discovered";

        // Get the characteristic
        QLowEnergyCharacteristic characteristic = m_service->characteristic(QBluetoothUuid(QUuid(CHARACTERISTIC_UUID)));
        if (characteristic.isValid()) {
            qDebug() << "Characteristic found, enabling notifications...";

            // Enable notifications
            QLowEnergyDescriptor notification = characteristic.descriptor(QBluetoothUuid(QUuid("00002902-0000-1000-8000-00805f9b34fb")));
            if (notification.isValid()) {
                m_service->writeDescriptor(notification, QByteArray::fromHex("0100"));
                qDebug() << "Notifications enabled for characteristic:" << characteristic.uuid().toString();
            } else {
                qWarning() << "Invalid notification descriptor for characteristic:" << characteristic.uuid().toString();
            }
        } else {
            qWarning() << "Characteristic not found:" << CHARACTERISTIC_UUID;
        }
    }
}

void BLEClient::errorReceived(QLowEnergyController::Error error)
{
    qWarning() << "Error:" << error;
    m_connectionStatus = "Error" + QString::number(error);
    emit connectionStatusChanged();
}

//for float variable
// void BLEClient::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
// {
//     qDebug() << "Characteristic changed:" << characteristic.uuid().toString();

//     if (characteristic.uuid() == QBluetoothUuid(QUuid(CHARACTERISTIC_UUID))) {
//         memcpy(&m_receivedFloat, newValue.constData(), sizeof(float));
//         qDebug() << "Received value:" << m_receivedFloat;
//         emit valueReceived(m_receivedFloat);
//     }
// }

//for string variable
// void BLEClient::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
// {
//     qDebug() << "Characteristic changed:" << characteristic.uuid().toString();

//     if (characteristic.uuid() == QBluetoothUuid(QUuid(CHARACTERISTIC_UUID))) {
//         // Convert the received data to a QString
//         m_receivedMessage = QString::fromUtf8(newValue);
//         qDebug() << "Received message:" << m_receivedMessage;

//         // Emit the signal
//         emit messageReceived(m_receivedMessage);
//     }
// }

//for double variable
// void BLEClient::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
// {
//     qDebug() << "Characteristic changed:" << characteristic.uuid().toString();

//     if (characteristic.uuid() == QBluetoothUuid(QUuid(CHARACTERISTIC_UUID))) {
//         // Parse the received double value
//         memcpy(&m_receivedDouble, newValue.constData(), sizeof(double));
//         qDebug() << "Received double:" << m_receivedDouble;

//         // Emit the signal
//         emit doubleReceived(m_receivedDouble);
//     }
// }

//for the int
// void BLEClient::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
// {
//     qDebug() << "Characteristic changed:" << characteristic.uuid().toString();

//     if (characteristic.uuid() == QBluetoothUuid(QUuid(CHARACTERISTIC_UUID))) {
//         // Parse the received integer value
//         memcpy(&m_receivedInt, newValue.constData(), sizeof(int));
//         qDebug() << "Received integer:" << m_receivedInt;

//         // Emit the signal
//         emit intReceived(m_receivedInt);
//     }
// }

//for the int16
void BLEClient::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    qDebug() << "Characteristic changed:" << characteristic.uuid().toString();

    if (characteristic.uuid() == QBluetoothUuid(QUuid(CHARACTERISTIC_UUID))) {
        // Parse the received uint16_t value
        memcpy(&m_receivedUInt16, newValue.constData(), sizeof(quint16));
        qDebug() << "Received uint16_t:" << m_receivedUInt16;

        // Emit the signal
        emit uint16Received(m_receivedUInt16);
    }
}
float BLEClient::receivedFloat() const
{
    return m_receivedFloat;
}

QString BLEClient::receivedMessage() const
{
    return m_receivedMessage;
}

double BLEClient::receivedDouble() const
{
    return m_receivedDouble;
}

int BLEClient::receivedInt() const
{
    return m_receivedInt;
}

quint16 BLEClient::receivedUInt16() const
{
    return m_receivedUInt16;
}
