#include "fbdeployer.h"

namespace Freetils {
    FbDeployer::FbDeployer(QObject *parent) : QObject(parent)
    {

    }

    FbDeployer::~FbDeployer()
    {
        workerThread.quit();
    }

    void FbDeployer::serve(QString rootFolder, QString fbxIp)
    {
        m_FbxIP = fbxIp;        

        //@todo check what FolderDialog returns on mac...
        //FolderDialog returns a string like file:// on unix or file:/// on windows...
        //so file://{/} is removed then prepend only one / for absolute root dir project path
        QRegularExpression regex("(file:\\/{2,3})");
        rootFolder.replace(regex, "");
        rootFolder.prepend("/");

        Server* server = new Server(nullptr, rootFolder, m_LocalPort);

        server->moveToThread(&workerThread);

        connect(&workerThread, &QThread::finished, server, &QObject::deleteLater);
        connect(this, &FbDeployer::operate, server, &Server::start);
        connect(this, &FbDeployer::terminate, server, &Server::quit);
        connect(server, &Server::resultReady, this, &FbDeployer::resultReady);
        connect(server, &Server::resultEnded, this, &FbDeployer::resultEnded);
        connect(m_FbDetector, &FbDetector::hostIpFounded, this, &FbDeployer::setHostIP);

        workerThread.start();

        emit operate();
    }

    void FbDeployer::setHostIP(QString ip)
    {
        qDebug() << "setHostIP " << ip;
        m_HostIP = ip;
    }

    void FbDeployer::deploy()
    {
        QJsonObject arg;
        QString manifestUrl = "http://192.168.1.101:" + QString::number(m_LocalPort);
        qDebug() << "Manifest url " << manifestUrl;

        arg[QStringLiteral("manifest_url")] = manifestUrl;
        arg[QStringLiteral("entry_point")] = "main";
        arg[QStringLiteral("wait")] = false;

        QString url = "http://" + m_FbxIP + "/pub/devel";
        qDebug() << "devel url " << url;
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
