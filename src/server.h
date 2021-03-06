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
        void qmlLog(QByteArray text, QString lvl);

    public slots:
        void phpStart();
        void phpQuit();
        void qmlStart();
        void qmlQuit();

        // QTcpServer interface
    protected:
        // /!\ this change with Qt version...
//        virtual void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

    private:
        QThreadPool pool;
        QString m_RootFolder;
        quint16 m_Port;
        QString m_HostIp;
        QProcess* m_Php = nullptr;
        QProcess* m_QmlScene = nullptr;
        QHostAddress* m_Address = nullptr;
        bool m_IsQmlStarted = false;

    private:
        void phpStarted();
        void phpReadyReadStandardOutput();
        void phpReadyReadStandardError();
        void phpErrorOccurred(QProcess::ProcessError error);
        void qmlErrorOccurred(QProcess::ProcessError error);        
        void qmlReadyReadStandardOutput();
        void qmlReadyReadStandardError();
    };
}

#endif // SERVER_H
