# Add the Qt modules 'quick' and 'bluetooth' to the project.
# This ensures the project has access to Qt Quick (for QML-based UIs) and Qt Bluetooth (for Bluetooth functionality).
QT += quick bluetooth

# You can make your code fail to compile if it uses deprecated APIs.
# Uncomment the following line to disable all APIs deprecated before Qt 6.0.0.
# This is useful for ensuring your code is future-proof and doesn't rely on outdated features.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# List of source files to be compiled.
# These are the C++ files that contain the implementation of your application.
SOURCES += \
        bleclient.cpp \  # Implementation file for the BLE (Bluetooth Low Energy) client.
        main.cpp        # Main application file, typically containing the `main()` function.

# List of resource files.
# This includes the QML resource file (qml.qrc), which bundles QML files, images, and other assets.
RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model.
# This is useful for Qt Creator to locate custom QML modules.
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules specifically for Qt Quick Designer.
# This is used by the Qt Quick Designer tool to locate QML modules during UI design.
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
# These lines define where the compiled binary will be installed on different platforms.
qnx: target.path = /tmp/$${TARGET}/bin  # For QNX systems, deploy to /tmp/{target}/bin.
else: unix:!android: target.path = /opt/$${TARGET}/bin  # For Unix-like systems (excluding Android), deploy to /opt/{target}/bin.
!isEmpty(target.path): INSTALLS += target  # If a target path is defined, add it to the list of installation paths.

# List of header files.
# These are the C++ header files that declare classes, functions, and variables.
HEADERS += \
    bleclient.h  # Header file for the BLE client, typically containing class definitions and declarations.
