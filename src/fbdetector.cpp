#include "fbdetector.h"

namespace Freetils {
    FbDetector::FbDetector(QObject *parent) : QObject(parent)
    {

    }

    void FbDetector::scan()
    {
        if (QNetworkInterface::IsRunning) {
            const QList<QNetworkInterface>netInf = QNetworkInterface::allInterfaces();

            for (const QNetworkInterface& interface : netInf) {

                const QNetworkInterface::InterfaceFlags flags = interface.flags();

                if (!(flags & QNetworkInterface::IsUp) ||
                    !(flags & QNetworkInterface::IsRunning) ||
                     (flags & QNetworkInterface::IsLoopBack) ||
                     (flags & QNetworkInterface::IsPointToPoint) ||
                    !(flags & QNetworkInterface::CanMulticast))
                    continue;

                if (!interface.isValid()) {
                    qDebug("interface not valid");
                    return;
                }

                 if ("enp3s0f1" == interface.name()) {
                     continue;
                 }

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

                 QPair<QUdpSocket*, const QNetworkInterface*>listenerPair;
                 listenerPair.first = socketListener;
                 listenerPair.second = &interface;

                 m_SocketListener.append(listenerPair);

                for (const QNetworkAddressEntry& entry : interface.addressEntries())
                {
                    QHostAddress address = entry.ip();

                    if (address.protocol() == QAbstractSocket::IPv4Protocol) {

                        QUdpSocket* socketSender = new QUdpSocket(this);
                        socketSender->setProxy(QNetworkProxy::NoProxy);

                        if (!socketSender->bind(address)) {
                            qWarning() << "can't bind : " << socketSender->errorString();
                        }

                        QPair<QUdpSocket*, const QNetworkInterface*>senderPair;
                        senderPair.first = socketSender;
                        senderPair.second = &interface;

                        connect(socketSender, &QUdpSocket::stateChanged, this, &FbDetector::senderStateChanged);
                        connect(socketSender,  &QUdpSocket::readyRead, this, &FbDetector::senderReceived);

                        m_SocketSender.append(senderPair);
                    }
                }
            }
        }

        for (auto sender : m_SocketSender) {

            for (auto networkAddress : sender.second->allAddresses()) {

                if ("127.0.0.1" == networkAddress.toString() || networkAddress.protocol() != QAbstractSocket::IPv4Protocol) {
                    continue;
                }

                QString message;

                message = QStringLiteral("M-SEARCH * HTTP/1.1\r\n");
                message += QString::fromLocal8Bit("HOST: %1:%2\r\n")
                    .arg(QLatin1String(m_ADDR4))
                    .arg(QString::number(m_Port));
                message += QStringLiteral("MAN: \"ssdp:discover\"\r\n");
                message += QStringLiteral("MX: 1\r\n");
                message += QString::fromLocal8Bit("ST: %1\r\n").arg(m_FbNt);
                message += QStringLiteral("\r\n");

                sender.first->writeDatagram(message.toLatin1().constData(), message.size(),
                QHostAddress(QLatin1String(m_ADDR4)), m_Port);
            }
        }
    }

    void FbDetector::senderReceived()
    {
        for (auto sender : m_SocketSender) {
            while (sender.first->hasPendingDatagrams()) {
                QNetworkDatagram datagram = sender.first->receiveDatagram();

                if (datagram.data().contains(m_FbNt)) {

                    //@todo detect device type (revolution ? mini 4K ? delta ? unsuported ? )
                    Device* device = new Device(datagram.senderAddress().toString(), "revolution");

                    if (!m_DevicesList.contains(device)) {
                        m_DevicesList.append(device);
                        emit scanned(QVariant(device->getIp()));
                    }
                }
            }
        }
    }

    void FbDetector::senderStateChanged(QAbstractSocket::SocketState state)
    {
        Q_UNUSED(state);

        for (auto sender : m_SocketSender) {

            sender.first->setMulticastInterface(*sender.second);

            if (!sender.first->joinMulticastGroup(QHostAddress(QLatin1String(m_ADDR4)), *sender.second)) {
                qWarning() << "sender failed to join :" << sender.first->errorString();
            }
        }
    }
}
