#ifndef CLIENT_H
#define CLIENT_H

#include <regex>
#include <iostream>
#include <QObject>
#include <QDebug>
#include <QRunnable>
#include <QThread>
#include <QTcpSocket>
#include <QBuffer>
#include <QFile>
#include <QDir>

class Client : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr, qintptr handle = 0);

signals:


    // QRunnable interface
public:
    void run();
private:
    qintptr handle;
};

#endif // CLIENT_H
