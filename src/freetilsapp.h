#ifndef FREETILSAPP_H
#define FREETILSAPP_H

#include <QObject>

#include "fbdetector.h"
#include "fbdeployer.h"
#include "device.h"

namespace Freetils {
    class FreeTilsApp : public QObject
    {
        Q_OBJECT
    public:
        FreeTilsApp(QObject *parent = nullptr);

        Q_INVOKABLE void detectDevices();
        Q_INVOKABLE void deployApp(QString rootFolder, int currentIndex);

    private:
        void newDeviceDetected(QString deviceAdress, QString hostAddress);

    private:
        FbDetector* m_FbDetector;
        FbDeployer* m_FbDeployer;
        QList<Device> m_DevicesList;

        QString cleanRootFolder(QString rootFolder);

    signals:
        void refreshStbList(QString deviceAdress);
    };
}

#endif // FREETILSAPP_H
