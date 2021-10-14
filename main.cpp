#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>

#include "src/fbdetector.h"
#include "src/fbdeployer.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setOrganizationName("galliume");
    app.setOrganizationDomain("https://github.com/galliume/");
    app.setApplicationName("FreeTils");
    app.setWindowIcon(QIcon(":/images/icon.png"));

    QQmlApplicationEngine engine;

    qmlRegisterType<Freetils::FbDetector>("com.galliume.FbDetector", 1, 0, "FbDetector");
    qmlRegisterType<Freetils::FbDeployer>("com.galliume.FbDeployer", 1, 0, "FbDeployer");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
