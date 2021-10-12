#include "fbdeployer.h"

namespace Freetils {
    FbDeployer::FbDeployer(QObject *parent) : QObject(parent)
    {
        qInfo() << "FbDeployer";
    }

    bool FbDeployer::serve(QString fbxIp)
    {

    }
}
