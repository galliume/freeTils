#ifndef FBDETECTOR_H
#define FBDETECTOR_H

#include <QObject>
#include <QNetworkInterface>
#include <QDebug>
#include <QUdpSocket>
#include <QNetworkProxy>
#include <QNetworkDatagram>
#include <QPair>
#include <QList>
#include "device.h"

namespace Freetils {
    class FbDetector : public QObject
    {
        Q_OBJECT
    public:
        explicit FbDetector(QObject *parent = nullptr);
        Q_INVOKABLE void scan();

    private:
        const char m_ADDR4[16] = "239.255.255.250";
        const quint16 m_Port = 1900;
        const char m_FbNt[10] = "fbx:devel";
        QString m_HostIP;

        QList<QPair<QUdpSocket*, const QNetworkInterface*>> m_SocketListener;
        QList<QPair<QUdpSocket*, const QNetworkInterface*>> m_SocketSender;

        QList<Device*> m_DevicesList;

    private slots:
//        void listenerReceived();
//        void listenerStateChanged(QAbstractSocket::SocketState state);
        void senderReceived();
        void senderStateChanged(QAbstractSocket::SocketState state);

    signals:
        void scanned(QVariant data);
        void hostIpFounded(QString ip);
    };
}
#endif // FBDETECTOR_H
