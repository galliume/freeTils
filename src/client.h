#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <QObject>
#include <QDebug>
#include <QRunnable>
#include <QThread>
#include <QTcpSocket>
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QRegularExpression>
#include <QProcess>

namespace Freetils {
    class Client : public QObject, public QRunnable
    {
        Q_OBJECT
    public:
        explicit Client(QObject *parent = nullptr, QString rootFolder = "", qintptr handle = 0);
        ~Client();
        QTcpSocket* getSocket() { return m_Socket; };

    signals:

        // QRunnable interface
    public:
        void run();
    private:
        QString m_RootFolder;
        qintptr handle;
        QTcpSocket* m_Socket = nullptr;
    };
}

#endif // CLIENT_H
