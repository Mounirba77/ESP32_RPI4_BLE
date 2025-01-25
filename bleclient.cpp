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
}

void BLEClient::serviceStateChanged(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::RemoteServiceDiscovered) {
        qDebug() << "Service details discovered";
    }
}

void BLEClient::errorReceived(QLowEnergyController::Error error)
{
    qWarning() << "Error:" << error;
    m_connectionStatus = "Error";
    emit connectionStatusChanged();
}
