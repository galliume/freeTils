#include "fbdeployer.h"

namespace Freetils {
    FbDeployer::FbDeployer(QObject *parent) : QObject(parent)
    {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        qRegisterMetaType<QPair<bool, QString>>("QPair<bool, QString>");
#endif
    }

    FbDeployer::~FbDeployer()
    {
        stop();
    }

    void FbDeployer::serve(QString rootFolder, QString fbxIp, QString hostIp)
    {
        m_Reply = nullptr;
        m_FbxIP = fbxIp;        
        m_HostIP = hostIp;
        m_RootFolder = rootFolder;

        Server* server = new Server(nullptr, m_RootFolder, m_HostIP, m_LocalPort);

        m_WorkerThread = new QThread();
        server->moveToThread(m_WorkerThread);

        connect(m_WorkerThread, &QThread::finished, server, &QObject::deleteLater);
        connect(this, &FbDeployer::operate, server, &Server::phpStart);
        connect(this, &FbDeployer::phpQuit, server, &Server::phpQuit);
        connect(server, &Server::resultReady, this, &FbDeployer::resultReady);
        connect(server, &Server::resultEnded, this, &FbDeployer::resultEnded);

        m_WorkerThread->start();

        emit operate();
    }

    void FbDeployer::deploy()
    {
        m_Qnam = new QNetworkAccessManager(this);
        m_Out = new QTcpSocket(this);
        m_Qml = new QTcpSocket(this);
        m_Err = new QTcpSocket(this);

        QJsonObject arg;
        QJsonObject params;
        QString manifestUrl = "http://" + m_HostIP + ":" + QString::number(m_LocalPort) + "/manifest.json";

        params[QStringLiteral("entry_point")] = "main";
        params[QStringLiteral("wait")] = false;
        params[QStringLiteral("manifest_url")] = manifestUrl;

        arg[QStringLiteral("jsonrpc")] = QStringLiteral("2.0");
        arg[QStringLiteral("method")] = "debug_qml_app";
        arg[QStringLiteral("params")] = params;

        QJsonDocument jsonDoc(arg);
        QString url = "http://" + m_FbxIP + "/pub/devel";

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        m_Qnam->setTransferTimeout(2000);
        m_Reply = m_Qnam->post(request, jsonDoc.toJson());

        connect(m_Qnam, &QNetworkAccessManager::finished, this, &FbDeployer::response);
        connect(m_Reply, &QNetworkReply::errorOccurred, this, &FbDeployer::errorOccurred);
    }

    void FbDeployer::connectADB(QString miniIP)
    {
        m_ADB = new QProcess();
        m_ADB->setProcessChannelMode(QProcess::MergedChannels);
        connect(m_ADB, &QProcess::errorOccurred, this, &FbDeployer::adbErrorOccured);
        connect(m_ADB, &QProcess::started, this, &FbDeployer::adbStarted);
        connect(m_ADB, &QProcess::readyReadStandardOutput, this, &FbDeployer::adbOutput);
        connect(m_ADB, &QProcess::readyReadStandardError, this, &FbDeployer::adbError);

        QStringList args;
        QString addr = miniIP;
        args << "connect" << miniIP;

        m_ADB->setProgram("adb");
        m_ADB->setArguments(args);
        m_ADB->start();
        m_ADB->waitForStarted();

    }

     void FbDeployer::deployToMini(QString miniIP, QString wsPort)
     {
         //QThread* thread = new QThread();
         QUrl url = QUrl("main.qml");
         qDebug() << "deployToMini to : " << url;
         QString addr = "ws://"+miniIP+":"+wsPort;
         qDebug() << addr;
         QNetworkRequest request = QNetworkRequest(QUrl(addr.toLatin1()));

         m_ADPSocket = new QWebSocket();
         connect(m_ADPSocket, &QWebSocket::connected, this, &FbDeployer::miniConnected);
         connect(m_ADPSocket, &QWebSocket::stateChanged, this, &FbDeployer::miniStateChanged);
         connect(m_ADPSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
             [=](QAbstractSocket::SocketError error){
             qDebug() << "error " << error;
             QString msg = "ws connection error ";
             msg.append(error);

             emit logged(msg, "err");

         });

         m_ADPSocket->open(request);
         //m_ADPSocket->moveToThread(thread);
     }

    void FbDeployer::startMini(QString miniIP, QString nameActivity)
    {
        m_miniIP = miniIP;
        m_ADB = new QProcess();
        m_ADB->setProcessChannelMode(QProcess::MergedChannels);
        connect(m_ADB, &QProcess::errorOccurred, this, &FbDeployer::adbErrorOccured);
        connect(m_ADB, &QProcess::started, this, &FbDeployer::adbStarted);
        connect(m_ADB, &QProcess::finished, this, &FbDeployer::adbFinished);
        connect(m_ADB, &QProcess::readyReadStandardOutput, this, &FbDeployer::adbOutput);
        connect(m_ADB, &QProcess::readyReadStandardError, this, &FbDeployer::adbError);

        //adb connect 192.168.1.9
        //adb shell
        //pm clear fr.freebox.qmllauncher
        //am start -a "android.intent.action.VIEW" -d "vodservice://6play"

        QStringList args;
        QString addr = miniIP;
        args << "shell" << "am" << "start" << "-a" << "android.intent.action.VIEW" <<  "-d" << "vodservice://" + nameActivity;

        m_ADB->setProgram("adb");
        m_ADB->setArguments(args);
        m_ADB->start();
        m_ADB->waitForStarted();
    }

    void FbDeployer::adbErrorOccured(QProcess::ProcessError error)
    {
        qDebug() << "ADB ERROR " << error;
        QString msg = "ADB error : ";
        msg.append(error);
        emit logged(msg, "err");
    }

    void FbDeployer::adbError()
    {
        QString msg = "ADB error : ";
        msg.append(m_ADB->readAllStandardError());
        emit logged(msg, "err");
    }

    void FbDeployer::adbStarted()
    {
        qDebug() << "ADB STARTED";

        emit logged("ADB started", "debug");
    }

    void FbDeployer::adbFinished(int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << "ADB fnished "<< exitCode << " " << exitStatus;
        QString msg = "mini 4k finished ";
        msg.append(exitCode);
        msg.append(exitStatus);
        emit logged(msg, "debug");

    }

    void FbDeployer::adbOutput()
    {
        QString msg = "mini 4k ";
        msg.append( m_ADB->readAllStandardOutput());
        emit logged(msg, "debug");
    }

    void FbDeployer::miniStateChanged(QAbstractSocket::SocketState state)
    {
        qDebug() << "state changed " << state;
        QString msg = "status changed on mini 4K ";
        msg.append(state);
        emit logged(msg, "debug");
    }

    void FbDeployer::miniErrorOccurred(QProcess::ProcessError error)
    {
        qDebug() << "error " << error;
        QString msg = "Error on mini 4K ";
        msg.append(error);
        emit logged(msg, "err");

    }

    void FbDeployer::miniConnected()
    {
        qDebug() << "connected on mini 4K";
        emit logged("connected on mini 4K", "debug");
        QJsonObject params;
        QString manifestUrl = "http://192.168.1.101:" + QString::number(m_LocalPort) + "/main.qml";
        qDebug() << "manifest url " << manifestUrl;
        params[QStringLiteral("entry_point")] = manifestUrl;
        QJsonDocument jsonDoc(params);
        qDebug() << jsonDoc;
        m_ADPSocket->sendTextMessage(jsonDoc.toJson());
    }

    void FbDeployer::launchQmlScene()
    {
        m_QmlWorkerThread = new QThread();
        //@todo rename server and separate php / qml process
        Server* server = new Server(nullptr, m_RootFolder, m_HostIP, m_LocalPort);

        server->moveToThread(m_QmlWorkerThread);

        connect(m_QmlWorkerThread, &QThread::finished, server, &QObject::deleteLater);
        connect(this, &FbDeployer::operateQML, server, &Server::qmlStart);
        connect(this, &FbDeployer::qmlQuit, server, &Server::qmlQuit);
        connect(server, &Server::resultReady, this, &FbDeployer::resultReady);
        connect(server, &Server::resultEnded, this, &FbDeployer::resultEnded);
        connect(server, &Server::qmlLog, this, &FbDeployer::log);

        m_QmlWorkerThread->start();

        emit operateQML();
    }

    void FbDeployer::errorOccurred(QNetworkReply::NetworkError code)
    {
        Q_UNUSED(code);
        emit logged("Error when connecting to stb", "err");
    }

    void FbDeployer::response(QNetworkReply *reply)
    {
        if(reply->error() == QNetworkReply::NoError)
        {
            QString contents = QString::fromUtf8(reply->readAll()); 

            QJsonDocument jsonDoc = QJsonDocument::fromJson(contents.toUtf8());

            if (!jsonDoc["error"].isUndefined()) {
                emit logged(jsonDoc["error"]["message"].toString(), "err");

                QPair<bool, QString>status;
                status.first = false;
                status.second = "STB is not ready, try again";
                emit deployed(status);

                this->stop();

                return;
            }

            if (m_Out->state() == QTcpSocket::SocketState::UnconnectedState) {
                m_Out->connectToHost(QHostAddress(m_FbxIP), jsonDoc["result"]["stdout_port"].toDouble());

                if (m_Out->isValid()) {
                    connect(m_Out, &QTcpSocket::readyRead, this, &FbDeployer::out);
                    connect(m_Out, &QTcpSocket::disconnected, this, &FbDeployer::socketOutDisconnected);
                }
            }

            if (m_Err->state() == QTcpSocket::SocketState::UnconnectedState) {
                m_Err->connectToHost(QHostAddress(m_FbxIP), jsonDoc["result"]["stderr_port"].toDouble());

                if (m_Err->isValid()) {
                    connect(m_Err, &QTcpSocket::readyRead, this, &FbDeployer::err);
                    connect(m_Err, &QTcpSocket::disconnected, this, &FbDeployer::socketErrDisconnected);
                }
            }

            if (m_Qml->state() == QTcpSocket::SocketState::UnconnectedState) {
                m_Qml->connectToHost(QHostAddress(m_FbxIP), jsonDoc["result"]["qml_port"].toDouble());

                if (m_Qml->isValid()) {
                    connect(m_Qml, &QTcpSocket::readyRead, this, &FbDeployer::qml);
                    connect(m_Qml, &QTcpSocket::disconnected, this, &FbDeployer::socketQmlDisconnected);
                }
            }
        }
        else
        {
            QString err = reply->errorString();
            emit logged(err, "err");
        }        
    }

    void FbDeployer::log(QByteArray text, QString lvl)
    {
        //@todo real ansi code parsing ?
        QRegularExpression regex("(?:\\\x1B\\[0m)(.+)(?:\\\x1B\\[0m)");
        QRegularExpressionMatch match = regex.match(text);

        if (match.hasMatch()) {
            QString matched = match.captured(1);

            QRegularExpression regexText("(?:\\\x1B\\[0m)(.+)");
            QRegularExpressionMatch matchBis = regexText.match(matched);

            if (matchBis.hasMatch()) {
                QString matchedBis = matchBis.captured(1);

                 if (matchBis.hasMatch()) {
                     matched = matchedBis;
                 }
            }

            if (text.contains("1;33m")) {
                lvl = "err";
            }

            emit logged(matched, lvl);
        } else {
            emit logged(text, lvl);
        }
    }

    void FbDeployer::out()
    {
        QByteArray out = m_Out->readAll();
        this->log(out);
    }

    void FbDeployer::err()
    {
        QByteArray err = m_Err->readAll();
        this->log(err);
    }

    void FbDeployer::qml()
    {
        QByteArray qml = m_Qml->readAll();
        this->log(qml);
    }

    void FbDeployer::socketOutDisconnected()
    {
         //emit logged("out socket disconnected", "info");
    }

    void FbDeployer::socketErrDisconnected()
    {
        //emit logged("err socket disconnected", "info");
    }

    void FbDeployer::socketQmlDisconnected()
    {
        //emit logged("qml socket disconnected", "info");
    }

    void FbDeployer::stop()
    {
        if (nullptr != m_Qml) {
            m_Qml->close();
        }

        if (nullptr != m_Err) {
            m_Err->close();
        }

        if (nullptr != m_Out) {
            m_Out->close();
        }

        m_Reply = nullptr;

        emit phpQuit();
        emit qmlQuit();

        emit logged("Bye.", "info");
    }

    void FbDeployer::resultReady(QPair<bool, QString>status)
    {
         emit deployed(status);
    }

    void FbDeployer::resultEnded(QPair<bool, QString>status)
    {
        if (nullptr != m_WorkerThread && m_WorkerThread->isRunning()) {
            m_WorkerThread->quit();
            if (!m_WorkerThread->wait(3000)) {
                m_WorkerThread->terminate();
            }
        }

        if (nullptr != m_QmlWorkerThread && m_QmlWorkerThread->isRunning()) {
            m_QmlWorkerThread->quit();
            if (!m_QmlWorkerThread->wait(3000)) {
                m_QmlWorkerThread->terminate();
            }
        }

        emit stopped(status);
    }
}
