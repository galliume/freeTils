QT += quick
QT += network
QT += core
QT += remoteobjects
QT += websockets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        src/client.cpp \
        src/fbdeployer.cpp \
        src/fbdetector.cpp \
        src/server.cpp

RESOURCES += \
    qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/client.h \
    src/fbdeployer.h \
    src/fbdetector.h \
    src/server.h

unix:RC_ICONS = += icons/icon.png
win32:RC_ICONS += icons/icon.ico
macx: ICON += icons/icon.icns
