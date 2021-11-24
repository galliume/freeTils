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
        this->quit();
        this->quitQML();
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
        if (m_IsQmlStarted) return;

        qDebug() << "qmlStart";
        m_QmlScene = new QProcess();

        connect(m_QmlScene, &QProcess::finished, this, &Server::qmlSceneStateChanged);
        connect(m_QmlScene, &QProcess::errorOccurred, this, &Server::qmlErrorOccurred);
        connect(m_QmlScene, &QProcess::readyReadStandardError, this, &Server::readyReadStandardError);
        connect(m_QmlScene, &QProcess::readyReadStandardOutput, this, &Server::readyReadStandardOutput);

        QStringList args;
        QString addr = "http://127.0.0.1:" + QString::number(m_Port) + "/main.qml";
        args << "-I" << "/home/gpercepied/Documents/workspace/freeTils/vendor/libfbxqml" << addr;
        m_QmlScene->start("qml", args);
        m_QmlScene->waitForStarted();
    }

    void Server::readyReadStandardError()
    {
        qDebug() << "Stantard Error : " << m_QmlScene->readAllStandardError();
    }

    void Server::readyReadStandardOutput()
    {
        qDebug() << "Standard Output : " << m_QmlScene->readAllStandardOutput();
    }

    void Server::qmlSceneStateChanged()
    {
        m_IsQmlStarted = (m_QmlScene->state() == QProcess::Running) ? true : false;;
        QPair<bool, QString>status;
        status.first = m_IsQmlStarted;
        status.second = m_QmlScene->readAllStandardOutput();

        if (!m_IsQmlStarted) {
            quitQML();
        }

        emit(resultEnded(status));
    }

    void Server::qmlErrorOccurred(QProcess::ProcessError error)
    {
        Q_UNUSED(error);
        QPair<bool, QString>status;
        status.first = false;
        status.second = m_QmlScene->readAllStandardError();

        quitQML();

        emit(resultEnded(status));    
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
