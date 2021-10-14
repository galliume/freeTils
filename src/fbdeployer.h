#ifndef FBDEPLOYER_H
#define FBDEPLOYER_H

#include "server.h"
#include "fbdetector.h"

#include <QObject>
#include <QDebug>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QNetworkProxy>
#include <QNetworkDatagram>
#include <QtRemoteObjects>
#include <QTcpServer>
#include <QTextStream>
#include <QThread>

namespace Freetils {
    class FbDeployer : public QObject
    {
        Q_OBJECT
        QThread workerThread;

    public:
        explicit FbDeployer(QObject *parent = nullptr);
        ~FbDeployer();

        Q_INVOKABLE void serve(QString rootFolder, QString fbxIp);
        Q_INVOKABLE void stop();

    private:
        const quint16 m_LocalPort = 9000;
        QString m_FbxIP;
        QString m_HostIP;

        QNetworkInterface m_Interface;
        QHostAddress m_Address;
        QRemoteObjectNode m_RemoteNode;
        QTcpServer* m_TcpServer;

        void deploy();

        FbDetector* m_FbDetector = nullptr;

    public slots:
        void resultReady(QPair<bool, QString>status);
        void resultEnded(QPair<bool, QString>status);
        void setHostIP(QString ip);

    signals:
        void operate();
        void terminate();
        void deployed(QVariant isDeployed, QVariant status);
        void stoped(QVariant isStoped, QVariant status);
    };
}
#endif // FBDEPLOYER_H
