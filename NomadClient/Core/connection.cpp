#include "connection.h"

Connection::Connection(QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected, this, &Connection::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Connection::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Connection::onDisconnected);

    connect(m_socket, &QTcpSocket::errorOccurred, this, &Connection::onErrorOccurred);
}

void Connection::connectToServer(const QString &host, quint16 port)
{
    qInfo() << "Conect to " << host << ":" << port;
    m_socket->connectToHost(host, port);
}

bool Connection::sendMessage(const QString &message)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        m_socket->write(message.toUtf8());
        m_socket->flush();
        return true;
    }
    else
    {
        qWarning() << "ERROR: Server not responding. Message was not sent.";
    }
    return false;
}

void Connection::onConnected()
{
    qInfo() << "Successful connection!";
}

void Connection::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    qInfo() << "Message received: " << QString::fromUtf8(data);
}

void Connection::onDisconnected()
{
    qInfo() << "Disconnect!";
}

void Connection::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qCritical() << "ERROR: Socket:" << m_socket->errorString();
}
