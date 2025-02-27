#ifndef BLECLIENT_H
#define BLECLIENT_H

#include <QObject>
#include <QLowEnergyController>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QLowEnergyService>
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

class BLEClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(float receivedFloat READ receivedFloat NOTIFY floatReceived)

public:
    explicit BLEClient(QObject *parent = nullptr);
    void connectToDevice(const QBluetoothDeviceInfo &deviceInfo);
    Q_INVOKABLE void sendData(const QByteArray &data); // New method to send data

    QString deviceName() const;
    QString connectionStatus() const;
    float receivedFloat() const;

signals:
    void deviceNameChanged();
    void connectionStatusChanged();
    void floatReceived(float value);

private slots:
    void deviceConnected();
    void deviceDisconnected();
    void serviceDiscovered(const QBluetoothUuid &serviceUuid);
    void serviceStateChanged(QLowEnergyService::ServiceState newState);
    void errorReceived(QLowEnergyController::Error error);
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);

private:
    QLowEnergyController *m_controller = nullptr;
    QLowEnergyService *m_service = nullptr;
    QString m_deviceName;
    QString m_connectionStatus;
    float m_receivedFloat = 0.0f;
    QLowEnergyCharacteristic m_rxCharacteristic; // Characteristic for receiving data
};

#endif // BLECLIENT_H
