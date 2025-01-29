// Include necessary Qt headers
#include <QGuiApplication>       // For the Qt GUI application framework
#include <QQmlApplicationEngine> // For loading and running QML files
#include <QQmlContext>           // For exposing C++ objects to QML
#include "bleclient.h"           // Include the BLEClient class header

// Main function
int main(int argc, char *argv[])
{
    // Enable high-DPI scaling for Qt versions below 6.0
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // Create a Qt GUI application instance
    QGuiApplication app(argc, argv);

    // Create a QML application engine to load and run QML files
    QQmlApplicationEngine engine;

    // Create an instance of the BLEClient class
    BLEClient bleClient;

    // Expose the BLEClient instance to QML as a context property
    // This allows QML to access the BLEClient object and its properties/methods
    engine.rootContext()->setContextProperty("bleClient", &bleClient);

    // Define the URL of the main QML file (loaded from the Qt resource system)
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    // Connect a lambda function to the objectCreated signal of the QML engine
    // This ensures the application exits if the QML file fails to load
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) // If the object creation failed and the URL matches
                QCoreApplication::exit(-1); // Exit the application with an error code
        },
        Qt::QueuedConnection); // Use queued connection to ensure the signal is processed asynchronously

    // Load the main QML file
    engine.load(url);

    // Create a QBluetoothAddress object with the ESP32's MAC address
    QBluetoothAddress address("94:54:C5:B1:79:1E");

    // Create a QBluetoothDeviceInfo object for the ESP32 device
    QBluetoothDeviceInfo deviceInfo(address, "ESP32", QBluetoothDeviceInfo::LowEnergyCoreConfiguration);

    // Connect to the ESP32 BLE device using the BLEClient instance
    bleClient.connectToDevice(deviceInfo);

    // Enter the Qt event loop and run the application
    return app.exec();
}
