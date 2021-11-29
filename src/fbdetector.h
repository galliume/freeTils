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
#include <QProcess>

#include "device.h"

namespace Freetils {
    class FbDetector : public QObject
    {
        Q_OBJECT
    public:
        explicit FbDetector(QObject *parent = nullptr);
        void scan();
        void scanAndroid();
        void senderReceived();

    private:
        const char m_ADDR4[16] = "239.255.255.250";
        const quint16 m_Port = 1900;
        const char m_FbNt[10] = "fbx:devel";

        QList<QPair<QUdpSocket*, const QNetworkInterface*>> m_SocketListener;
        QList<QPair<QUdpSocket*, const QNetworkInterface*>> m_SocketSender;

        QProcess* m_ADB = nullptr;

        void adbErrorOccured(QProcess::ProcessError error);
        void adbError();
        void adbStarted();
        void adbOutput();

    private slots:
        void senderStateChanged(QAbstractSocket::SocketState state);

    signals:
        void newDeviceDetected(QString deviceAddress, QString hostAddress);
    };
}
#endif // FBDETECTOR_H
