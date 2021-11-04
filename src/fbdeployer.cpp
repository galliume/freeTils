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

    }

    void FbDeployer::serve(QString rootFolder, QString fbxIp, QString hostIp)
    {
        m_Reply = nullptr;
        m_FbxIP = fbxIp;        
        m_HostIP = hostIp;

        Server* server = new Server(nullptr, rootFolder, m_HostIP, m_LocalPort);

        server->moveToThread(&workerThread);

        connect(&workerThread, &QThread::finished, server, &QObject::deleteLater);
        connect(this, &FbDeployer::operate, server, &Server::start);
        connect(this, &FbDeployer::serverQuit, server, &Server::quit);
        connect(server, &Server::resultReady, this, &FbDeployer::resultReady);
        connect(server, &Server::resultEnded, this, &FbDeployer::resultEnded);

        workerThread.start();

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

    void FbDeployer::launch(QString rootFolder)
    {
        qDebug() << "launch rootFolder" << rootFolder;
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
                this->stop();

                QPair<bool, QString>status;
                status.first = false;
                status.second = "STB is not ready, try again";
                emit deployed(status);
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

    void FbDeployer::log(QByteArray text)
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

            QString lvl = "info";

            if (text.contains("1;33m")) {
                lvl = "err";
            }

            emit logged(matched, lvl);
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
        m_Qml->close();
        m_Err->close();
        m_Out->close();

        m_Reply = nullptr;

        emit serverQuit();
        workerThread.quit();
    }

    void FbDeployer::resultReady(QPair<bool, QString>status)
    {
         emit deployed(status);
    }

    void FbDeployer::resultEnded(QPair<bool, QString>status)
    {
        emit stopped(status);
    }
}
