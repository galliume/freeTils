
#include "client.h"

namespace Freetils {
    Client::Client(QObject *parent, QString rootFolder, qintptr handle) : QObject(parent), QRunnable(), handle(handle)
    {
        m_RootFolder = rootFolder;
    }

    void Client::run()
    {
        QTcpSocket* socket = new QTcpSocket(nullptr);//multithread => no parent

        if (!socket->setSocketDescriptor(handle)) {
            qCritical() << socket->errorString();
            delete socket;

            return;
        }

        socket->waitForReadyRead();

        QByteArray request = socket->readAll();
        QString file;
        QRegularExpression regex("(?!GET \\/) (.+) HTTP");
        QRegularExpressionMatch match = regex.match(request);

        if (match.hasMatch()) {
            file = match.captured(1);
        }

        if (0 == file.indexOf("/")) {
            file.remove(0, 1);
        }

        QByteArray data;
        QDir::setCurrent(m_RootFolder);
        QDir projectDir;
        QFile path(projectDir.absoluteFilePath(file));
        QString httpCode = "200 OK";

        if (!path.open(QIODevice::ReadOnly)) {
            qWarning() << "failed to open file " << qUtf8Printable(path.fileName());
            httpCode = "404 Not Found";
        }

        while (!path.atEnd()) {
            QByteArray buffer = path.read(file.size());
            data.append(buffer);
        }

        QString const lenght = "Content-Length: " + QString::number(data.length()) + "\r\n";
        QByteArray response;

        response.append("HTTP/1.1 "+ httpCode.toLocal8Bit() + "\r\n");
        response.append("Content-Type: text/html\r\n");
        response.append(lenght.toLocal8Bit());
        response.append("Connection: close\r\n");
        response.append("\r\n");
        response.append(data);

        socket->write(response);
        socket->waitForBytesWritten();
        socket->close();
        socket->deleteLater();
    }
}
