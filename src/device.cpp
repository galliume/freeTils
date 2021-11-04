#include "device.h"

namespace Freetils {
    Device::Device(QString ip, STB_TYPES type) : m_Ip(ip), m_Type(type)
    {

    }

    bool Device::operator==(const Device& device) const
    {
        return device.m_Ip == m_Ip;
    }
}
