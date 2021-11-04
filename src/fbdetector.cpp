#include "fbdetector.h"

namespace Freetils {
    FbDetector::FbDetector(QObject *parent) : QObject(parent)
    {

    }

    void FbDetector::scanAndroid()
    {
        qDebug() << "scanAndroid";

        m_ADB = new QProcess();

        connect(m_ADB, &QProcess::errorOccurred, this, &FbDetector::adbErrorOccured);
        connect(m_ADB, &QProcess::started, this, &FbDetector::adbStarted);
        connect(m_ADB, &QProcess::readyReadStandardOutput, this, &FbDetector::adbOutput);
        connect(m_ADB, &QProcess::readyReadStandardError, this, &FbDetector::adbError);

        QStringList args;

        args << "connect" << "192.168.1.9:5555";
        m_ADB->start("adb", args);
        m_ADB->waitForStarted();

        //adb connect 192.168.1.9
        //adb shell
        //pm clear fr.freebox.qmllauncher
        //am start -a "android.intent.action.VIEW" -d "vodservice://6play"
    }

    void FbDetector::adbErrorOccured(QProcess::ProcessError error)
    {
        qDebug() << "ADB ERROR " << error;
    }

    void FbDetector::adbError()
    {
        qDebug() << "ADB adbError " << m_ADB->readAllStandardError();
    }

    void FbDetector::adbStarted()
    {
        qDebug() << "ADB STARTED";
    }

    void FbDetector::adbOutput()
    {
        qDebug() << "ADB adbOutput " << m_ADB->readAllStandardOutput();
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
                    qDebug() << "interface not valid " << interface.name();
                    return;
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
                    emit newDeviceDetected(datagram.senderAddress().toString(), datagram.destinationAddress().toString());
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
