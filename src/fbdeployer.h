#ifndef FBDEPLOYER_H
#define FBDEPLOYER_H

#include "crow.h"

#include <QObject>
#include <QDebug>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QNetworkProxy>
#include <QNetworkDatagram>
#include <QtRemoteObjects>
#include <QTcpServer>
#include <QTextStream>

namespace Freetils {
    class FbDeployer : public QObject
    {
        Q_OBJECT
    public:
        explicit FbDeployer(QObject *parent = nullptr);

        Q_INVOKABLE bool serve(QString fbxIp);

    private:
        const quint16 m_LocalPort = 9000;

        QNetworkInterface m_Interface;
        QHostAddress m_Address;
        QRemoteObjectNode m_RemoteNode;
        QTcpServer* m_TcpServer;

    public slots:
        void handle(QHttpRequest *req, QHttpResponse *resp);

    signals:

    };
}
#endif // FBDEPLOYER_H
