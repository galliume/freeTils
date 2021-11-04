#include "freetilsapp.h"

namespace Freetils {
    FreeTilsApp::FreeTilsApp(QObject *parent) : QObject(parent)
    {
        m_FbDetector = new FbDetector();

        connect(m_FbDetector, &FbDetector::newDeviceDetected, this, &FreeTilsApp::newDeviceDetected);
    }

    void FreeTilsApp::detectDevices()
    {
        m_FbDetector->scan();
    }

    void FreeTilsApp::newDeviceDetected(QString deviceAdress)
    {
        //@todo detect device type (revolution ? mini 4K ? delta ? unsuported ? )
        Device device = Device(deviceAdress, Device::STB_TYPES::REVOLUTION);

        if (!m_DevicesList.contains(&device)) {
            m_DevicesList.append(&device);

            emit refreshStbList(deviceAdress);
        }
    }
}
