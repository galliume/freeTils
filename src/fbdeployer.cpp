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
        qDebug() << fbxIp;

        //@todo why remove file:// ?
        rootFolder.remove(0, 7);
        Server* server = new Server(nullptr, rootFolder, m_LocalPort);

        server->moveToThread(&workerThread);

        connect(&workerThread, &QThread::finished, server, &QObject::deleteLater);
        connect(this, &FbDeployer::operate, server, &Server::start);
        connect(server, &Server::resultReady, this, &FbDeployer::handleResults);

        workerThread.start();

        deploy();
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

    void FbDeployer::handleResults(const QString &)
    {
        qDebug() << "handle Results";
    }

}
