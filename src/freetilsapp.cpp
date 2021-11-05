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

        if (!m_DevicesList.contains(device)) {
            m_DevicesList.append(device);

            emit refreshStbList(deviceAddress, device.getIcon());
        }
    }

    void FreeTilsApp::deployApp(QString rootFolder, int index)
    {
        --index;//gap of one with the fbx list because of "Select Freebox" at index 0
        Device device = m_DevicesList.at(index);
        rootFolder = cleanRootFolder(rootFolder);

        m_FbDeployer->serve(rootFolder, device.getIp(), device.getHostIp());
    }

    void FreeTilsApp::serverDeployed(QPair<bool, QString>status)
    {
        if (status.first) {
            m_FbDeployer->deploy();
        }

        emit serverUpdated(status.first, status.second);
    }

    void FreeTilsApp::serverStopped(QPair<bool, QString>status)
    {
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
