#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QThread>

#include "client.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr, QString rootFolder = "", quint16 port = 9000);

signals:
    void resultReady(const QString &result);

public slots:
    void start();
    void quit();

    // QTcpServer interface
protected:
    // /!\ this change with Qt version...
    virtual void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

private:
    QThreadPool pool;
    QString m_RootFolder;
    quint16 m_Port;
};

#endif // SERVER_H
