#include "core.h"

QCore::QCore(QObject *parent) : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &QCore::onNewConnection);
}


bool QCore::startServer(quint16 port)
{
    if (!m_tcpServer->listen(QHostAddress::Any, port))
    {
        qCritical() << "ERROR: Server couldn't start. Error message:" << m_tcpServer->errorString();
        return false;
    }
    qInfo() << "Server has been running!" << port;
    return true;
}

void QCore::onNewConnection()
{
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();

    Client *client = new Client(clientSocket, this);
    m_clients.append(client);

    qInfo() << "New connection, make new client worker:" << client->peerAddress();


    connect(client, &Client::messageReceived, this, &QCore::onReadyRead);
    connect(client, &Client::disconnected, this, &QCore::onClientDisconnected);
}

void QCore::onReadyRead(Client *sender, const QByteArray &data)
{
    if (!sender) return;

    qInfo() << "NewMessage:" << data;

    for (Client *client : m_clients)
    {
        if (client == sender) continue;

        client->sendMessage(data);
    }
}

void QCore::onClientDisconnected(Client *client)
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    qInfo() << "Client: " << client->peerAddress() << " leave from the chat.";
    m_clients.removeAll(client);
    client->deleteLater();
}
