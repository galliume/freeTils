#include "server.h"

Server::Server(QObject *parent, QString rootFolder, quint16 port) : QTcpServer(parent)
{
    m_RootFolder = rootFolder;
    m_Port = port;
    pool.setMaxThreadCount(5);
}

void Server::start()
{
    qInfo() << this << " start " << QThread::currentThread();

    if (this->listen(QHostAddress::Any, m_Port)) {
        qInfo() << " server started on " << m_Port;
    } else {
        qCritical() << this->errorString();
    }

    emit resultReady("OKOKOK");
}

void Server::quit()
{
    this->close();
    qInfo() << "quit";
}

void Server::incomingConnection(qintptr handle)
{
    //  /!\ depends on qt version... check doc !
    qInfo() << "Incomming connection " << handle << " on " << QThread::currentThread();

    Client* client = new Client(nullptr, m_RootFolder, handle);
    client->setAutoDelete(true);
    pool.start(client);

}
