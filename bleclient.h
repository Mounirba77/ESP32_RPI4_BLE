#ifndef BLECLIENT_H
#define BLECLIENT_H

#include <QObject>
#include <QLowEnergyController>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QLowEnergyService>
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

class BLEClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(float receivedFloat READ receivedFloat NOTIFY floatReceived)
    Q_PROPERTY(QString receivedMessage READ receivedMessage NOTIFY messageReceived)
    Q_PROPERTY(double receivedDouble READ receivedDouble NOTIFY doubleReceived)

public:
    explicit BLEClient(QObject *parent = nullptr);
    void connectToDevice(const QBluetoothDeviceInfo &deviceInfo);

    QString deviceName() const;
    QString connectionStatus() const;
    float receivedFloat() const;
    QString receivedMessage() const;
    double receivedDouble() const;

signals:
    void deviceNameChanged();
    void connectionStatusChanged();
    void floatReceived(float value);
    void messageReceived(const QString &message);
    void doubleReceived(double value);

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
    QString m_receivedMessage;
    double m_receivedDouble;

};

#endif // BLECLIENT_H
