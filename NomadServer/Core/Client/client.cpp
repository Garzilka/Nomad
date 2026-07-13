#include "client.h"
#include <QDebug>

Client::Client(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket)
{
    m_socket->setParent(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
}

Client::~Client()
{
    qInfo() << "Client: " << peerAddress() << " has been removed";
}

void Client::sendMessage(const QByteArray &data)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    m_socket->write(data);
    m_socket->flush();
}

QString Client::peerAddress() const
{
    return m_socket->peerAddress().toString();
}

void Client::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    emit messageReceived(this, data);
}

void Client::onDisconnected()
{
    emit disconnected(this);
}
