#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QThreadPool>

#include "client.h"

namespace Freetils {
    class Server : public QTcpServer
    {
        Q_OBJECT
    public:
        explicit Server(QObject *parent = nullptr, QString rootFolder = "", QString hostIp = "127.0.0.1", quint16 port = 9000);
        ~Server();

    signals:
        void resultReady(QPair<bool, QString>status);
        void resultEnded(QPair<bool, QString>status);

    public slots:
        void start();
        void startQML();
        void quit();
        void quitQML();

        // QTcpServer interface
    protected:
        // /!\ this change with Qt version...
//        virtual void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

    private:
        QThreadPool pool;
        QString m_RootFolder;
        quint16 m_Port;
        QString m_HostIp;
        QProcess* m_Php;
        QProcess* m_QmlScene;
        QHostAddress* m_Address;
        bool m_IsQmlStarted = false;
    private:
        void phpStateChanged();
        void qmlSceneStateChanged();
        void qmlErrorOccurred(QProcess::ProcessError error);
    };
}

#endif // SERVER_H
