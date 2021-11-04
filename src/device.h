#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QDebug>

namespace Freetils {
    class Device
    {

    public:

        enum STB_TYPES {
            REVOLUTION,
            DELTA,
            MINI4K
        };

        Device(QString ip, QString hostIp, STB_TYPES type);
        QString const getIp() const { return m_Ip; };
        QString const getHostIp() const { return m_HostIp; };
        STB_TYPES getType() { return m_Type; };
        bool operator==(const Device& device) const;

    private:
        QString m_Ip;
        QString m_HostIp;
        STB_TYPES m_Type;
    };
}

#endif // DEVICE_H
