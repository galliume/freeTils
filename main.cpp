#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "src/fbdetector.h"
#include "src/fbdeployer.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<Freetils::FbDetector>("com.gcconantc.FbDetector", 1, 0, "FbDetector");
    qmlRegisterType<Freetils::FbDeployer>("com.gcconantc.FbDeployer", 1, 0, "FbDeployer");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
