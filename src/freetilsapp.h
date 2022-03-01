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
        Q_INVOKABLE void launchQmlScene(QString rootFolder);
        Q_INVOKABLE void deployAppMini(QString miniIP, QString wsPort);
        Q_INVOKABLE void stop();
        Q_INVOKABLE void connectADB(QString miniIP);
        Q_INVOKABLE void startMini(QString miniIP, QString nameActivity);

    private:
        void newDeviceDetected(QString deviceAddress, QString hostAddress);
        void serverDeployed(QPair<bool, QString>status);
        void serverStopped(QPair<bool, QString>status);
        void logger(QString log, QString lvl);
        QString cleanRootFolder(QString rootFolder);

    private:
        FbDetector* m_FbDetector = nullptr;
        FbDeployer* m_FbDeployer = nullptr;
        QList<Device> m_DevicesList;
        bool m_QmlScene = false;
        bool m_QmlSceneRunning = false;
        bool m_DeployedToStb = false;

    signals:
        void refreshStbList(QString deviceAddress, QString iconAddress);
        void serverUpdated(QVariant isDeployed, QVariant status);
        void logged(QString log, QString lvl);
    };
}

#endif // FREETILSAPP_H
