#include "server.h"

namespace Freetils {
    Server::Server(QObject *parent, QString rootFolder, quint16 port) : QTcpServer(parent)
    {
        m_RootFolder = rootFolder;
        m_Port = port;
        pool.setMaxThreadCount(5);
    }

    void Server::start()
    {
        QPair<bool, QString>status;

        //@todo put IP from freebox network => 192.168.101
        if (this->listen(QHostAddress::Any, m_Port)) {
            status.first = true;
            status.second = "Server started on http://192.168.101:" + QString::number(m_Port);
            qInfo() << status;
        } else {
            status.first = false;
            status.second = "Error : " + this->errorString();
            qCritical() << status;
        }

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
        //  /!\ depends on qt version... check doc !
        qInfo() << "Incomming connection " << handle << " on " << QThread::currentThread();

        Client* client = new Client(nullptr, m_RootFolder, handle);
        client->setAutoDelete(true);
        pool.start(client);

    }
}
