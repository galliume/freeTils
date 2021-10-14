#include "fbdeployer.h"

namespace Freetils {
    FbDeployer::FbDeployer(QObject *parent) : QObject(parent)
    {
        qInfo() << "FbDeployer";
    }

    FbDeployer::~FbDeployer()
    {
        workerThread.quit();
        workerThread.wait();
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
        connect(server, &Server::resultReady, this, &FbDeployer::handleResults);

        workerThread.start();

        emit operate();
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

    void FbDeployer::handleResults(QPair<bool, QString>status)
    {
        emit deployed(status.first, status.second);
    }

}
