#ifndef FBDEPLOYER_H
#define FBDEPLOYER_H

#include <QObject>

namespace Freetils {
    class FbDeployer : public QObject
    {
        Q_OBJECT
    public:
        explicit FbDeployer(QObject *parent = nullptr);

    signals:

    };
}
#endif // FBDEPLOYER_H
