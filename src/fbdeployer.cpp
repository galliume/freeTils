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

        //@todo check what FolderDialog returns on mac...
        //FolderDialog returns a string like file:// on unix or file:/// on windows...
        //so file://{/} is removed then if linux OS prepend only one / for absolute root dir project path
        QRegularExpression regex("(file:\\/{2,3})");
        rootFolder.replace(regex, "");

        //@todo check for MacOS
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
        QString manifestUrl = m_HostIP + ":" + QString::number(m_LocalPort);
        qDebug() << "Manifest url " << manifestUrl;

        arg[QStringLiteral("manifest_url")] = manifestUrl;
        arg[QStringLiteral("entry_point")] = "main";
        arg[QStringLiteral("wait")] = false;

        QJsonDocument jsonDoc(arg);
        QByteArray data = jsonDoc.toJson();

        QString url = "http://" + m_FbxIP + "/pub/devel";
        qDebug() << "devel url " << url;
        qDebug() << "params " << data;

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        m_Qnam.setTransferTimeout(3000);
        m_Reply = m_Qnam.post(request, data);

        connect(&m_Qnam, &QNetworkAccessManager::finished, this, &FbDeployer::response);
        connect(m_Reply, &QNetworkReply::errorOccurred, this, &FbDeployer::errorOccurred);
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
            qDebug() << contents;
        }
        else
        {
            QString err = reply->errorString();
            qDebug() << err;
        }

        reply->deleteLater();
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
