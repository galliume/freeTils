#include "device.h"

Device::Device(QString ip, QString type) : m_Ip(ip), m_Type(type)
{

}

bool Device::operator==(const Device& device) const
{
    return device.m_Ip == m_Ip;
}
