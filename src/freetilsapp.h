#ifndef FREETILSAPP_H
#define FREETILSAPP_H

#include <QObject>

#include "fbdetector.h"
#include "device.h"

namespace Freetils {
    class FreeTilsApp : public QObject
    {
        Q_OBJECT
    public:
        FreeTilsApp(QObject *parent = nullptr);

        Q_INVOKABLE void detectDevices();

        void newDeviceDetected(QString deviceAdress);

    private:
        FbDetector* m_FbDetector;
        QList<Device*> m_DevicesList;

    signals:
        void refreshStbList(QString deviceAdress);
    };
}

#endif // FREETILSAPP_H
