#include "server.h"

namespace Freetils {
    Server::Server(QObject *parent, QString rootFolder, QString hostIp, quint16 port) : QTcpServer(parent)
    {
        m_RootFolder = rootFolder;
        m_Port = port;
        m_HostIp = hostIp;
        pool.setMaxThreadCount(1);
    }

    Server::~Server()
    {
        //this->quit();
    }

    void Server::start()
    {
        m_Address = new QHostAddress();
        m_Address->setAddress(m_HostIp);

//        if (this->listen(address, m_Port)) {
//            status.first = true;
//            status.second = "Server started on " + address.toString() + ":" + QString::number(m_Port);
//        } else {
//            status.first = false;
//            status.second = "Error : " + this->errorString();
//            qCritical() << status;
//        }

        m_Php = new QProcess();

        connect(m_Php, &QProcess::finished, this, &Server::phpStateChanged);

        QStringList args;
        QString addr = m_Address->toString() + ":" + QString::number(m_Port);
        args << "-S" << addr << "-t" << m_RootFolder;
        m_Php->start("php", args);
        m_Php->waitForStarted();
    }

    void Server::phpStateChanged()
    {
        QPair<bool, QString>status;
        status.first = true;
        status.second = "Server started on " + m_Address->toString() + ":" + QString::number(m_Port);
        emit resultReady(status);
    }

    void Server::startQML()
    {
        qDebug() << "qmlStart";
        m_QmlScene = new QProcess();

        connect(m_QmlScene, &QProcess::finished, this, &Server::qmlSceneStateChanged);
        connect(m_QmlScene, &QProcess::errorOccurred, this, &Server::qmlErrorOccurred);

        QStringList args;
        QString addr = "http://127.0.0.1:" + QString::number(m_Port) + "/loader.qml";
        args << "-I" << "/home/gpercepied/Documents/workspace/freeTils/vendor/libfbxqml" << addr;
        qDebug() << "qml scene " << args;
        m_QmlScene->startCommand("qml -I /home/gpercepied/Documents/workspace/freeTils/vendor/libfbxqml http://127.0.0.1:9000/loader.qml");
    }

    void Server::qmlSceneStateChanged()
    {
        qDebug() << "qml scene started " << m_QmlScene->state();
    }

    void Server::qmlErrorOccurred(QProcess::ProcessError error)
    {
        qDebug() << "qml scene error occured " << error;
        quitQML();
    }


    void Server::quit()
    {
        m_Php->close();
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

    void Server::quitQML()
    {
        qDebug() << "quitQML";
        m_QmlScene->close();
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

//    void Server::incomingConnection(qintptr handle)
//    {
//        Client* client = new Client(nullptr, m_RootFolder, handle);
//        this->addPendingConnection(client->getSocket());
//        client->setAutoDelete(true);
//        pool.start(client);
//    }
}
