#ifndef BLECLIENT_H
#define BLECLIENT_H

#include <QObject>
#include <QLowEnergyController>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QLowEnergyService>

class BLEClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)

public:
    explicit BLEClient(QObject *parent = nullptr);
    void connectToDevice(const QBluetoothDeviceInfo &deviceInfo);

    QString deviceName() const;
    QString connectionStatus() const;

signals:
    void deviceNameChanged();
    void connectionStatusChanged();

private slots:
    void deviceConnected();
    void deviceDisconnected();
    void serviceDiscovered(const QBluetoothUuid &serviceUuid);
    void serviceStateChanged(QLowEnergyService::ServiceState newState);
    void errorReceived(QLowEnergyController::Error error);

private:
    QLowEnergyController *m_controller = nullptr;
    QLowEnergyService *m_service = nullptr;
    QString m_deviceName;
    QString m_connectionStatus;
};

#endif // BLECLIENT_H
