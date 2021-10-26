#include "server.h"

namespace Freetils {
    Server::Server(QObject *parent, QString rootFolder, QString hostIp, quint16 port) : QTcpServer(parent)
    {
        m_RootFolder = rootFolder;
        m_Port = port;
        m_HostIp = hostIp;
        pool.setMaxThreadCount(50);
    }

    void Server::start()
    {
        QPair<bool, QString>status;

        QHostAddress address;
        address.setAddress(m_HostIp);

//        if (this->listen(address, m_Port)) {
//            status.first = true;
//            status.second = "Server started on " + address.toString() + ":" + QString::number(m_Port);
//        } else {
//            status.first = false;
//            status.second = "Error : " + this->errorString();
//            qCritical() << status;
//        }

        QProcess php;
        QStringList args;
        QString addr = address.toString() + ":" + QString::number(m_Port);
        args << "-S" << addr << "-t" << m_RootFolder;
        php.start("php", args);
        php.waitForStarted();

        status.first = true;
        status.second = "Server started on " + address.toString() + ":" + QString::number(m_Port);

        emit resultReady(status);
    }

    void Server::quit()
    {
        this->close();

        QPair<bool, QString>status;

        if (this->isListening()) {
            status.first = false;
            status.second = "Error still listening";
        } else {
            status.first = true;
            status.second = "Server stoped";
        }

        emit(resultEnded(status));
    }

    void Server::incomingConnection(qintptr handle)
    {
        Client* client = new Client(nullptr, m_RootFolder, handle);
        //this->addPendingConnection(client->getSocket());
        client->setAutoDelete(true);
        pool.start(client);
    }
}
