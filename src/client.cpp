
#include "client.h"

Client::Client(QObject *parent, qintptr handle) : QObject(parent), QRunnable(), handle(handle)
{

}

void Client::run()
{
    qInfo() << this << " run " << QThread::currentThread();

    QTcpSocket* socket = new QTcpSocket(nullptr);//multithread => no parent

    if (!socket->setSocketDescriptor(handle)) {
        qCritical() << socket->errorString();
        delete socket;

        return;
    }

    socket->waitForReadyRead();
    QByteArray request = socket->readAll();

    qInfo() << " request " << request.length();

    QByteArray data("<html><head><title>freeTils</title></head><body>hello world!</body></html>");
    QString lenght = "Content-Length: " + QString::number(data.length()) + "\r\n";

    QByteArray response;
    response.append("HTTP/1.1 200 OK\r\n");
    response.append("Content-Type: text/html\r\n");
    response.append(lenght.toStdString());
    response.append("Connection: close\r\n");
    response.append("\r\n");
    response.append(data);

    socket->write(response);
    socket->waitForBytesWritten();
    socket->close();
    socket->deleteLater();

    qInfo() << this << " done " << QThread::currentThread();
}
