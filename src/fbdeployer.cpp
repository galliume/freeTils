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
        qDebug() << fbxIp;

        //@todo why remove file:// ?
        rootFolder.remove(0, 7);
        Server* server = new Server(nullptr, rootFolder, m_LocalPort);

        server->moveToThread(&workerThread);

        connect(&workerThread, &QThread::finished, server, &QObject::deleteLater);
        connect(this, &FbDeployer::operate, server, &Server::start);
        connect(server, &Server::resultReady, this, &FbDeployer::handleResults);

        workerThread.start();

        emit operate();
    }

    void FbDeployer::handleResults(const QString &)
    {
        qDebug() << "handle Results";
    }

}
