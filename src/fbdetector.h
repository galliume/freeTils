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

namespace Freetils {
    class FbDetector : public QObject
    {
        Q_OBJECT
    public:
        explicit FbDetector(QObject *parent = nullptr);
        Q_INVOKABLE void Scan();

    private:
        const char m_ADDR4[16] = "239.255.255.250";
        const quint16 m_Port = 1900;
        const char FREEBOX_DEVICE_NT[10] = "fbx:devel";

        QList<QPair<QUdpSocket*, QNetworkInterface*>> m_SocketListener;
        QList<QPair<QUdpSocket*, QNetworkInterface*>> m_SocketSender;

    private slots:
        void listenerReceived();
        void senderReceived();
        void listenerStateChanged(QAbstractSocket::SocketState state);
        void senderStateChanged(QAbstractSocket::SocketState state);

    signals:
        void scanned(QVariant data);

    };
}
#endif // FBDETECTOR_H
