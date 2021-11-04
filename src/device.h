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

        Device(QString ip, STB_TYPES type);
        QString getIp() { return m_Ip; };
        STB_TYPES getType() { return m_Type; };
        bool operator==(const Device& device) const;

    private:
        QString m_Ip;
        STB_TYPES m_Type;
    };
}

#endif // DEVICE_H
