
#include "client.h"

namespace Freetils {
    Client::Client(QObject *parent, QString rootFolder, qintptr handle) : QObject(parent), QRunnable(), handle(handle)
    {
        m_RootFolder = rootFolder;
    }

    Client::~Client()
    {
        m_Socket->close();
        this->disconnect();
    }

    void Client::run()
    {
        m_Socket = new QTcpSocket(nullptr);//multithread => no parent

        if (!m_Socket->setSocketDescriptor(handle)) {
            qCritical() << m_Socket->errorString();
            delete m_Socket;
            return;
        }

        m_Socket->waitForReadyRead();

        QByteArray request = m_Socket->readAll();
        QString file;
        QRegularExpression regex("(?!GET \\/) (.+) HTTP");
        QRegularExpressionMatch match = regex.match(request);

        if (match.hasMatch()) {
            file = match.captured(1);
        }

        if (0 == file.indexOf("/")) {
            file.remove(0, 1);
        }

        QDir::setCurrent(m_RootFolder);
        QDir projectDir;
        QFile path(projectDir.absoluteFilePath(file));
        QString httpCode = "200 OK";
        QByteArray data, response;

        if (!path.open(QIODevice::ReadOnly)) {
            qWarning() << "failed to open file " << qUtf8Printable(path.fileName());
            httpCode = "404 Not Found";
            data = "";
        } else {
            data = path.readAll();
        }

        if (path.fileName().contains(".js")) {
            QProcess gzip;
            QStringList args("-c");

            gzip.start("gzip", args);

            if (!gzip.waitForStarted()) {
                qWarning() << "can't use gzip";
            }

            gzip.write(data);
            gzip.closeWriteChannel();

            if (!gzip.waitForFinished()) {
                qWarning() << "can't gzip";
            }

            //data = gzip.readAll();
        }

        QString length = "Content-Length: " + QString::number(data.size()) + "\r\n";

        response.append("HTTP/1.1 "+ httpCode.toLocal8Bit() + "\r\n");

        if (path.fileName().contains(".js")) {
            response.append("Content-Type: application/javascript\r\n");
            //response.append("Accept-Ranges: bytes\r\n");
            //response.append("Accept-Encoding: gzip\r\n");
            //response.append("Content-Encoding: gzip\r\n");
        } else {
            response.append("Accept-Ranges: none\r\n");
        }

        //response.append("Content-Type: text/plain\r\n");
        response.append("Connection: keep-alive\r\n");
        response.append("Keep-Alive: timeout=5\r\n");
        response.append(length.toLocal8Bit());
        //response.append("Cache-Control: no-cache\r\n");
        //response.append("Pragma: no-cache\r\n");
        response.append("\r\n");
        response.append(data);

        m_Socket->write(response);

        m_Socket->waitForBytesWritten();
        m_Socket->close();
        m_Socket->deleteLater();
    }
}
