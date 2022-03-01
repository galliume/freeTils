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
        void deployToMini(QString miniIP, QString wsPort);
        void connectADB(QString miniIP);
        void launchQmlScene();
        void log(QByteArray text, QString lvl = "info");        
        void serve(QString rootFolder, QString fbxIp, QString hostIp);
        void startMini(QString miniIP, QString nameActivity);

        Q_INVOKABLE void stop();

    private:
        const quint16 m_LocalPort = 9000;
        QString m_FbxIP;
        QString m_HostIP;
        QString m_RootFolder;
        QNetworkAccessManager* m_Qnam = nullptr;
        QNetworkReply* m_Reply = nullptr;

        QNetworkInterface m_Interface;
        QHostAddress m_Address;
        QRemoteObjectNode m_RemoteNode;
        QTcpServer* m_TcpServer = nullptr;
        QTcpSocket* m_Out = nullptr;
        QTcpSocket* m_Err = nullptr;
        QTcpSocket* m_Qml = nullptr;

        QThread* m_WorkerThread = nullptr;
        QThread* m_QmlWorkerThread = nullptr;
        QProcess* m_ADB = nullptr;
        QString m_miniIP;
        QWebSocket* m_ADPSocket;

    public slots:
        void resultReady(QPair<bool, QString>status);
        void resultEnded(QPair<bool, QString>status);
        void response(QNetworkReply *reply);
        void errorOccurred(QNetworkReply::NetworkError code);
        void miniConnected();
        void miniErrorOccurred(QProcess::ProcessError error);
        void miniStateChanged(QAbstractSocket::SocketState state);
        void adbErrorOccured(QProcess::ProcessError error);
        void adbError();
        void adbStarted();
        void adbOutput();
        void adbFinished(int exitCode, QProcess::ExitStatus exitStatus);

    signals:
        void operate();
        void phpQuit();
        void deployed(QPair<bool, QString>status);
        void stopped(QPair<bool, QString>status);
        void operateQML();
        void qmlQuit();
        void logged(QString log, QString lvl);
    };
}
#endif // FBDEPLOYER_H
