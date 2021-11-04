#include "freetilsapp.h"

namespace Freetils {
    FreeTilsApp::FreeTilsApp(QObject *parent) : QObject(parent)
    {
        m_FbDetector = new FbDetector();
        connect(m_FbDetector, &FbDetector::newDeviceDetected, this, &FreeTilsApp::newDeviceDetected);

        m_FbDeployer = new FbDeployer();
    }

    void FreeTilsApp::detectDevices()
    {
        m_FbDetector->scan();
    }

    void FreeTilsApp::newDeviceDetected(QString deviceAdress, QString hostAddress)
    {
        //@todo detect device type (revolution ? mini 4K ? delta ? unsuported ? )
        Device device = Device(deviceAdress, hostAddress, Device::STB_TYPES::REVOLUTION);

        if (!m_DevicesList.contains(device)) {
            m_DevicesList.append(device);

            emit refreshStbList(deviceAdress);
        }
    }

    void FreeTilsApp::deployApp(QString rootFolder, int index)
    {
        --index;
        Device device = m_DevicesList.at(index);
        rootFolder = cleanRootFolder(rootFolder);

        m_FbDeployer->serve(rootFolder, device.getIp(), device.getHostIp());
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
