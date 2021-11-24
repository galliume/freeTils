#ifndef FBDEPLOYER_H
#define FBDEPLOYER_H

#include "server.h"
#include "fbdetector.h"

#include <QObject>
#include <QDebug>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QtRemoteObjects>
#include <QTcpServer>
#include <QThread>
#include <QOperatingSystemVersion>
#include <QWebSocket>

namespace Freetils {
    class FbDeployer : public QObject
    {
        Q_OBJECT
        QThread* workerThread;

    public:
        explicit FbDeployer(QObject *parent = nullptr);
        ~FbDeployer();

        void out();
        void err();
        void qml();
        void socketOutDisconnected();
        void socketErrDisconnected();
        void socketQmlDisconnected();
        void deploy();
        void launchQmlScene();
        void log(QByteArray text, QString lvl = "info");

        void serve(QString rootFolder, QString fbxIp, QString hostIp);
        Q_INVOKABLE void stop();

    private:
        const quint16 m_LocalPort = 9000;
        QString m_FbxIP;
        QString m_HostIP;
        QString m_RootFolder;
        QNetworkAccessManager* m_Qnam;
        QNetworkReply* m_Reply;

        QNetworkInterface m_Interface;
        QHostAddress m_Address;
        QRemoteObjectNode m_RemoteNode;
        QTcpServer* m_TcpServer;
        QTcpSocket* m_Out;
        QTcpSocket* m_Err;
        QTcpSocket* m_Qml;

    public slots:
        void resultReady(QPair<bool, QString>status);
        void resultEnded(QPair<bool, QString>status);
        void response(QNetworkReply *reply);
        void errorOccurred(QNetworkReply::NetworkError code);

    signals:
        void operate();
        void operateQML();
        void serverQuit();
        void deployed(QPair<bool, QString>status);
        void stopped(QPair<bool, QString>status);
        void logged(QString log, QString lvl);
    };
}
#endif // FBDEPLOYER_H
