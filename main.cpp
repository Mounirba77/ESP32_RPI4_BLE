#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "bleclient.h"
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    BLEClient bleClient;
    engine.rootContext()->setContextProperty("bleClient", &bleClient);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    // Example: Connect to a BLE device (replace with your ESP32's address)
    QBluetoothAddress address("94:54:C5:B1:79:1E");
    QBluetoothDeviceInfo deviceInfo(address, "ESP32", QBluetoothDeviceInfo::LowEnergyCoreConfiguration);
    bleClient.connectToDevice(deviceInfo);
    return app.exec();
}
