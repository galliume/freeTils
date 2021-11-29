#include "server.h"

namespace Freetils {
    Server::Server(QObject *parent, QString rootFolder, QString hostIp, quint16 port) : QTcpServer(parent)
    {
        m_RootFolder = rootFolder;
        m_Port = port;
        m_HostIp = hostIp;
    }

    Server::~Server()
    {

    }

    void Server::start()
    {
        m_Address = new QHostAddress();
        m_Address->setAddress(m_HostIp);

        m_Php = new QProcess();
        m_Php->setProcessChannelMode(QProcess::MergedChannels);

        connect(m_Php, &QProcess::finished, this, &Server::phpStateChanged);
        connect(m_Php, &QProcess::errorOccurred, this, &Server::phpErrorOccurred);
        connect(m_Php, &QProcess::readyReadStandardError, this, &Server::phpReadyReadStandardError);
        connect(m_Php, &QProcess::readyReadStandardOutput, this, &Server::phpReadyReadStandardOutput);

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

    void Server::phpReadyReadStandardError()
    {
        emit qmlLog(m_Php->readAllStandardError(), "err");
    }

    void Server::phpReadyReadStandardOutput()
    {
        emit qmlLog(m_Php->readAllStandardOutput(), "info");
    }

    void Server::phpErrorOccurred(QProcess::ProcessError error)
    {
        Q_UNUSED(error);
        QPair<bool, QString>status;
        status.first = false;
        status.second = m_Php->readAllStandardError();

        emit resultEnded(status);
    }

    void Server::phpQuit()
    {
        m_Php->close();
        //this->close();

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

    void Server::startQML()
    {
        m_QmlScene = new QProcess();
        m_QmlScene->setProcessChannelMode(QProcess::MergedChannels);

        connect(m_QmlScene, &QProcess::finished, this, &Server::qmlQuit);
        connect(m_QmlScene, &QProcess::errorOccurred, this, &Server::qmlErrorOccurred);
        connect(m_QmlScene, &QProcess::readyReadStandardError, this, &Server::qmlReadyReadStandardError);
        connect(m_QmlScene, &QProcess::readyReadStandardOutput, this, &Server::qmlReadyReadStandardOutput);

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

    void Server::qmlSceneStateChanged()
    {
        m_IsQmlStarted = (m_QmlScene->state() == QProcess::Running) ? true : false;
        QPair<bool, QString>status;
        status.first = m_IsQmlStarted;
        status.second = m_QmlScene->readAllStandardError();

        emit resultEnded(status);
    }

    void Server::qmlReadyReadStandardError()
    {
        emit qmlLog(m_QmlScene->readAllStandardError(), "err");
    }

    void Server::qmlReadyReadStandardOutput()
    {
        emit qmlLog(m_QmlScene->readAllStandardOutput(), "info");
    }

    void Server::qmlErrorOccurred(QProcess::ProcessError error)
    {
        Q_UNUSED(error);
        QPair<bool, QString>status;
        status.first = false;
        status.second = m_QmlScene->readAllStandardError();

        emit resultEnded(status);
    }

    void Server::qmlQuit()
    {
        qDebug() << "QUIT QML";
        m_QmlScene->close();
        //emit phpQuit();
    }
}
