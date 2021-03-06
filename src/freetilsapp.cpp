#include "freetilsapp.h"

namespace Freetils {
    FreeTilsApp::FreeTilsApp(QObject *parent) : QObject(parent)
    {
        //@todo inject dependancies
        m_FbDetector = new FbDetector();
        connect(m_FbDetector, &FbDetector::newDeviceDetected, this, &FreeTilsApp::newDeviceDetected);

        m_FbDeployer = new FbDeployer();
        connect(m_FbDeployer, &FbDeployer::deployed, this, &FreeTilsApp::serverDeployed);
        connect(m_FbDeployer, &FbDeployer::stopped, this, &FreeTilsApp::serverStopped);
        connect(m_FbDeployer, &FbDeployer::logged, this, &FreeTilsApp::logger);
        connect(m_FbDeployer, &FbDeployer::adbConnected, this, &FreeTilsApp::startMini);
    }

    void FreeTilsApp::detectDevices()
    {
        m_FbDetector->scan();
        //m_FbDetector->scanAndroid();
    }

    void FreeTilsApp::newDeviceDetected(QString deviceAddress, QString hostAddress)
    {
        //@todo detect device type (revolution ? mini 4K ? delta ? unsuported ? )
        Device device = Device(deviceAddress, hostAddress, Device::STB_TYPES::REVOLUTION);

        if (!m_DevicesList.contains(device) && !device.getIp().endsWith(".254")) {
            m_DevicesList.append(device);

            emit refreshStbList(deviceAddress, device.getIcon());
        }
    }

    void FreeTilsApp::deployApp(QString rootFolder, int index)
    {
        if (0 <= index && m_DevicesList.size() >= index) {
            m_QmlScene = false;
            --index;//gap of one with the fbx list because of "Select Freebox" at index 0
            Device device = m_DevicesList.at(index);
            rootFolder = cleanRootFolder(rootFolder);

            m_FbDeployer->serve(rootFolder, device.getIp(), device.getHostIp());
        } else {
            emit serverUpdated(false, "Stb [" + QString::number(index) + "] does not exists");
        }
    }

    void FreeTilsApp::deployTo4k(QString miniIP, QString wsPort, QString nameActivity)
    {
        m_MiniIP = miniIP;
        m_MiniWsPort = wsPort;
        m_MiniNameActivity = nameActivity;

        m_FbDeployer->connectADB(m_MiniIP);
    }

    void FreeTilsApp::startMini()
    {
        m_FbDeployer->startMini(m_MiniIP, m_MiniNameActivity);
    }

    void FreeTilsApp::deployAppMini()
    {
        m_FbDeployer->deployToMini(m_MiniIP, m_MiniWsPort);
    }

    void FreeTilsApp::launchQmlScene(QString rootFolder)
    {
        m_QmlScene = true;
        rootFolder = cleanRootFolder(rootFolder);
        m_FbDeployer->serve(rootFolder, "127.0.0.1", "127.0.0.1");
    }

    void FreeTilsApp::serverDeployed(QPair<bool, QString>status)
    {
        if (status.first) {
            if (!m_QmlScene) {
                if (!m_DeployedToStb) {
                    m_DeployedToStb = true;
                    m_FbDeployer->deploy();
                    emit serverUpdated(status.first, status.second);
                }
            } else {
                if (!m_QmlSceneRunning) {
                    m_FbDeployer->launchQmlScene();
                    m_QmlSceneRunning = true;
                }
            }
        }
    }

    void FreeTilsApp::serverStopped(QPair<bool, QString>status)
    {
        m_QmlSceneRunning = false;
        m_DeployedToStb = false;
        emit serverUpdated(status.first, status.second);
    }

    void FreeTilsApp::stop()
    {
        m_FbDeployer->stop();
    }

    void FreeTilsApp::logger(QString log, QString lvl)
    {
        emit logged(log, lvl);
    }

    QString FreeTilsApp::cleanRootFolder(QString rootFolder)
    {
        //FolderDialog returns a string like file:// on unix or file:/// on windows...
        //so file://{/} is removed then if linux OS prepend only one / for absolute root dir project path
        QRegularExpression regex("(file:\\/{2,3})");
        rootFolder.replace(regex, "");

        if(QOperatingSystemVersion::Windows != QOperatingSystemVersion::currentType()) {
            rootFolder.prepend("/");
        }

        return rootFolder;
    }
}
