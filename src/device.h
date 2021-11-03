#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QDebug>

class Device
{

public:
    Device(QString ip, QString type);
    QString getIp() { return m_Ip; };
    QString getType() { return m_Type; };
    bool operator==(const Device& device) const;
private:
    QString m_Ip;
    QString m_Type;
};

#endif // DEVICE_H
