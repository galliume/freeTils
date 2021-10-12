#ifndef FBDEPLOYER_H
#define FBDEPLOYER_H

#include "server.h"

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

        Q_INVOKABLE void serve(QString fbxIp);

    private:
        const quint16 m_LocalPort = 9000;

        QNetworkInterface m_Interface;
        QHostAddress m_Address;
        QRemoteObjectNode m_RemoteNode;
        QTcpServer* m_TcpServer;

    public slots:
        void handleResults(const QString &);

    signals:
        void operate();
    };
}
#endif // FBDEPLOYER_H
