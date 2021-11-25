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
        //this->quitQML();
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

        m_Php->setProgram("php");
        m_Php->setArguments(args);
        m_Php->start();

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
        m_QmlScene = new QProcess();
        m_QmlScene->setProcessChannelMode(QProcess::MergedChannels);

        connect(m_QmlScene, &QProcess::finished, this, &Server::qmlSceneStateChanged);
        connect(m_QmlScene, &QProcess::errorOccurred, this, &Server::qmlErrorOccurred);
        connect(m_QmlScene, &QProcess::readyReadStandardError, this, &Server::readyReadStandardError);
        connect(m_QmlScene, &QProcess::readyReadStandardOutput, this, &Server::readyReadStandardOutput);

        QStringList args;
        //@todo make it configurable
        QString libfbxqml = m_RootFolder + "/vendor/libfbxqml";
        QString addr = "http://127.0.0.1:" + QString::number(m_Port) + "/main.qml";
        args << "-I" << libfbxqml << addr;

        m_QmlScene->setProgram("qml");
        m_QmlScene->setArguments(args);
        m_QmlScene->start();
        m_QmlScene->waitForStarted();
    }

    void Server::readyReadStandardError()
    {
        emit qmlLog(m_QmlScene->readAllStandardError(), "err");
    }

    void Server::readyReadStandardOutput()
    {
        emit qmlLog(m_QmlScene->readAllStandardOutput(), "info");
    }

    void Server::qmlSceneStateChanged()
    {
        m_IsQmlStarted = (m_QmlScene->state() == QProcess::Running) ? true : false;
        QPair<bool, QString>status;
        status.first = m_IsQmlStarted;
        status.second = m_QmlScene->readAllStandardError();

        if (!m_IsQmlStarted) {
            quitQML();
        }

        emit resultEnded(status);
    }

    void Server::qmlErrorOccurred(QProcess::ProcessError error)
    {
        Q_UNUSED(error);
        QPair<bool, QString>status;
        status.first = false;
        status.second = m_QmlScene->readAllStandardError();

        quitQML();

        emit resultEnded(status);
    }


    void Server::quit()
    {
        m_Php->close();
        this->close();
        m_Php = nullptr;

        QPair<bool, QString>status;

        if (this->isListening()) {
            status.first = false;
            status.second = "Error still listening";
        } else {
            status.first = true;
            status.second = "Server stoped";
        }

        emit resultEnded(status);
    }

    void Server::quitQML()
    {
        m_QmlScene->terminate();
        m_QmlScene->kill();

        this->close();
        m_QmlScene = nullptr;

        QPair<bool, QString>status;

        if (this->isListening()) {
            status.first = false;
            status.second = "Error still listening";
        } else {
            status.first = true;
            status.second = "Server stoped";
        }

        emit resultEnded(status);
    }

//    void Server::incomingConnection(qintptr handle)
//    {
//        Client* client = new Client(nullptr, m_RootFolder, handle);
//        this->addPendingConnection(client->getSocket());
//        client->setAutoDelete(true);
//        pool.start(client);
//    }
}
