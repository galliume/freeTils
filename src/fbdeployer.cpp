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
        workerThread.quit();
    }

    void FbDeployer::serve(QString rootFolder, QString fbxIp, QString hostIp)
    {
        m_FbxIP = fbxIp;        
        m_HostIP = hostIp;

        //FolderDialog returns a string like file:// on unix or file:/// on windows...
        //so file://{/} is removed then if linux OS prepend only one / for absolute root dir project path
        QRegularExpression regex("(file:\\/{2,3})");
        rootFolder.replace(regex, "");

        if(QOperatingSystemVersion::Windows != QOperatingSystemVersion::currentType()) {
            rootFolder.prepend("/");
        }

        Server* server = new Server(nullptr, rootFolder, m_HostIP, m_LocalPort);

        server->moveToThread(&workerThread);

        connect(&workerThread, &QThread::finished, server, &QObject::deleteLater);
        connect(this, &FbDeployer::operate, server, &Server::start);
        connect(this, &FbDeployer::terminate, server, &Server::quit);
        connect(server, &Server::resultReady, this, &FbDeployer::resultReady);
        connect(server, &Server::resultEnded, this, &FbDeployer::resultEnded);

        workerThread.start();

        emit operate();
    }

    void FbDeployer::deploy()
    {
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

        m_Qnam.setTransferTimeout(0);
        m_Reply = m_Qnam.post(request, jsonDoc.toJson());

        connect(&m_Qnam, &QNetworkAccessManager::finished, this, &FbDeployer::response);
        connect(m_Reply, &QNetworkReply::errorOccurred, this, &FbDeployer::errorOccurred);
    }

    void FbDeployer::launch(QString rootFolder)
    {
        qDebug() << "launch";
    }

    void FbDeployer::errorOccurred(QNetworkReply::NetworkError code)
    {
        qWarning() << "Error occured " << code;
    }

    void FbDeployer::response(QNetworkReply *reply)
    {
        if(reply->error() == QNetworkReply::NoError)
        {
            QString contents = QString::fromUtf8(reply->readAll()); 

            QJsonDocument jsonDoc = QJsonDocument::fromJson(contents.toUtf8());

            m_Out = new QTcpSocket(this);
            m_Out->connectToHost(QHostAddress(m_FbxIP), jsonDoc["result"]["stdout_port"].toDouble());

            if (m_Out->isValid()) {
                connect(m_Out, &QTcpSocket::readyRead, this, &FbDeployer::out);
                //connect(socket, &QWebSocket::disconnected, this, &FbDeployer::onSocketDisconnected);
            }

            m_Err = new QTcpSocket(this);
            m_Err->connectToHost(QHostAddress(m_FbxIP), jsonDoc["result"]["stderr_port"].toDouble());

            if (m_Err->isValid()) {
                connect(m_Err, &QTcpSocket::readyRead, this, &FbDeployer::err);
                //connect(socket, &QWebSocket::disconnected, this, &FbDeployer::onSocketDisconnected);
            }

            m_Qml = new QTcpSocket(this);
            m_Qml->connectToHost(QHostAddress(m_FbxIP), jsonDoc["result"]["qml_port"].toDouble());

            if (m_Qml->isValid()) {
                connect(m_Qml, &QTcpSocket::readyRead, this, &FbDeployer::qml);
                //connect(socket, &QWebSocket::disconnected, this, &FbDeployer::onSocketDisconnected);
            }
        }
        else
        {
            QString err = reply->errorString();
            qDebug() << err;
        }

        reply->deleteLater();
    }

    void FbDeployer::out()
    {
        QByteArray out = m_Out->readAll();
        qDebug() << "Output : " << out;
        emit logged(QVariant(out));
    }

    void FbDeployer::err()
    {
        QByteArray err = m_Err->readAll();
        qDebug() << "Error : " << err;
        emit logged(QVariant(err));
    }

    void FbDeployer::qml()
    {
        QByteArray qml = m_Qml->readAll();
        emit logged(QVariant(qml));
        qDebug() << "QML : " << qml;
    }

    void FbDeployer::stop()
    {
        emit terminate();
    }

    void FbDeployer::resultReady(QPair<bool, QString>status)
    {
        if (status.first) {
            deploy();
        }

        emit deployed(status.first, status.second);
    }

    void FbDeployer::resultEnded(QPair<bool, QString>status)
    {
        if (status.first) {
            workerThread.quit();
        }

        emit stoped(status.first, status.second);
    }
}
