#include "device.h"

namespace Freetils {
    Device::Device(QString ip, QString hostIp, STB_TYPES type) : m_Ip(ip), m_HostIp(hostIp), m_Type(type)
    {

    }

    bool Device::operator==(const Device& device) const
    {
        return device.getIp() == m_Ip;
    }

    QString Device::getIcon()
    {
        QString icon = "";

        if (getType() == Device::STB_TYPES::REVOLUTION) {
            icon = "http://" + m_Ip + ":" + QString::number(m_SchemaPort) + "/" + m_IconUrl;
        }

        return icon;
    }
}
