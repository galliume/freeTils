#include "fbdetector.h"

namespace Freetils {
    FbDetector::FbDetector(QObject *parent) : QObject(parent)
    {
        qInfo() << "FbDetector";
    }

    void FbDetector::Scan()
    {

        if (QNetworkInterface::IsRunning) {
            QList<QNetworkInterface>netInf = QNetworkInterface::allInterfaces();

            for (QNetworkInterface& interface : netInf) {

                QNetworkInterface::InterfaceFlags flags = interface.flags();

                if (!(flags & QNetworkInterface::IsUp) ||
                    !(flags & QNetworkInterface::IsRunning) ||
                     (flags & QNetworkInterface::IsLoopBack) ||
                     (flags & QNetworkInterface::IsPointToPoint) ||
                    !(flags & QNetworkInterface::CanMulticast))
                    continue;

                 qInfo() << "interface " << interface.name();

                 QUdpSocket* socketListener = new QUdpSocket(this);

                 if (!socketListener->bind(QHostAddress(QLatin1String(m_ADDR4)), m_Port,
                             QAbstractSocket::ShareAddress |
                             QAbstractSocket::ReuseAddressHint)) {
                       qWarning() << "can't bind : " << socketListener->errorString();
                   }

                 socketListener->setMulticastInterface(interface);
                 if (!socketListener->joinMulticastGroup(QHostAddress(QLatin1String(m_ADDR4)), interface)) {
                    qWarning() << "failed to join multicast " << interface << ":" << socketListener->errorString();
                 }

                 connect(socketListener, &QUdpSocket::stateChanged, this, &FbDetector::listenerStateChanged);
                 connect(socketListener,  &QUdpSocket::readyRead, this, &FbDetector::listenerReceived);

                 QPair<QUdpSocket*, QNetworkInterface*>listenerPair;
                 listenerPair.first = socketListener;
                 listenerPair.second = &interface;

                 m_SocketListener << listenerPair;

                if (!interface.isValid()) {
                    qDebug("interface not valid");
                    return;
                }

                for (const QNetworkAddressEntry& entry : interface.addressEntries())
                {
                    QHostAddress address = entry.ip();

                    if (address.protocol() == QAbstractSocket::IPv4Protocol) {

                        qDebug() << "\t " << address.toString();

                        QUdpSocket* socketSender = new QUdpSocket(this);
                        socketSender->setProxy(QNetworkProxy::NoProxy);

                        QPair<QUdpSocket*, QNetworkInterface*>senderPair;
                        senderPair.first = socketSender;
                        senderPair.second = &interface;

                        connect(socketSender, &QUdpSocket::stateChanged, this, &FbDetector::senderStateChanged);
                        connect(socketSender,  &QUdpSocket::readyRead, this, &FbDetector::senderReceived);

                        m_SocketSender << senderPair;
                    }
                }
            }
        }

        for (auto sender : m_SocketSender) {

            sender.first->setProxy(QNetworkProxy::NoProxy);

            foreach(const QNetworkAddressEntry networkAddress, sender.second->addressEntries()) {

                const QHostAddress hostAddress = networkAddress.ip();

                if ("127.0.0.1" == hostAddress.toString()|| hostAddress.protocol() != QAbstractSocket::IPv4Protocol) {
                    qDebug() << "local or not ipv4 " << hostAddress.toString();
                    continue;
                }

                qDebug() <<"binding to " << hostAddress.toString();

                if (!sender.first->bind(hostAddress)) {
                    qWarning() << "can't bind : " << sender.first->errorString();
                } else {

                    sender.first->setMulticastInterface(*sender.second);
                    qDebug() << hostAddress.toString();

                    QString message;

                    message = QStringLiteral("M-SEARCH * HTTP/1.1\r\n");
                    message += QString::fromLocal8Bit("HOST: %1:%2\r\n")
                        .arg(QLatin1String(m_ADDR4))
                        .arg(QString::number(m_Port));
                    message += QStringLiteral("MAN: \"ssdp:discover\"\r\n");
                    message += QStringLiteral("MX: 1\r\n");
                    message += QString::fromLocal8Bit("ST: %1\r\n").arg("fromHere");
                    message += QStringLiteral("\r\n");

                    qint64 success = sender.first->writeDatagram(message.toLatin1().constData(), message.size(),
                    QHostAddress(QLatin1String(m_ADDR4)), m_Port);

                    qInfo() << "Bytes sent " << success;
                }
            }
        }

        emit scanned(QVariant("192.161.1.12"));
    }


    void FbDetector::listenerReceived()
    {
        qInfo() << "Reading pending datagrams from listener";

        for (auto listener : m_SocketListener) {
            while (listener.first->hasPendingDatagrams()) {
                QNetworkDatagram datagram = listener.first->receiveDatagram();
                qInfo() << "read datagram from "  << datagram.senderAddress() << "::"<<datagram.senderPort();
                qInfo() << datagram.data();
            }
        }
    }

    void FbDetector::listenerStateChanged(QAbstractSocket::SocketState state)
    {
        for (auto listener : m_SocketListener) {

            listener.first->setMulticastInterface(*listener.second);

            if (!listener.first->joinMulticastGroup(QHostAddress(QLatin1String(m_ADDR4)), *listener.second)) {
                qWarning() << "listener failed to join :" << listener.first->errorString();
            } else {
                qInfo() << " join from listener " << state;
            }
        }
    }

    void FbDetector::senderReceived()
    {
        qInfo() << "Reading pending datagrams from sender";

        for (auto sender : m_SocketSender) {
            while (sender.first->hasPendingDatagrams()) {
                QNetworkDatagram datagram = sender.first->receiveDatagram();
                qInfo() << "read datagram from "  << datagram.senderAddress() << "::"<<datagram.senderPort();
                qInfo() << datagram.data();
            }
        }
    }


    void FbDetector::senderStateChanged(QAbstractSocket::SocketState state)
    {
        for (auto sender : m_SocketSender) {

            sender.first->setMulticastInterface(*sender.second);

            if (!sender.first->joinMulticastGroup(QHostAddress(QLatin1String(m_ADDR4)), *sender.second)) {
                qWarning() << "sender failed to join :" << sender.first->errorString();
            } else {
                qInfo() << " join from sender " << state;
            }
        }
    }
}
