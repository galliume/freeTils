#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <QObject>
#include <QDebug>
#include <QRunnable>
#include <QThread>
#include <QTcpSocket>
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QRegularExpression>

class Client : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr, QString rootFolder = "", qintptr handle = 0);

signals:


    // QRunnable interface
public:
    void run();
private:
    QString m_RootFolder;
    qintptr handle;
};

#endif // CLIENT_H
