#include "core.h"
#include "Core/DataBase/repository.h"

QCore::QCore(QObject *parent) : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &QCore::onNewConnection);
    QRepository::getInstance();
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


    connect(client, &Client::OnMessageReceived, this, &QCore::onReadyRead);
    connect(client, &Client::disconnected, this, &QCore::onClientDisconnected);
}

void QCore::onReadyRead(Client *sender, QJsonObject& data)
{
    if (!sender) return;
    ETypeOfMessage TypeMessage = static_cast<ETypeOfMessage>(data["TypeMessage"].toInt());

    if(TypeMessage == ETypeOfMessage::Login || TypeMessage == ETypeOfMessage::Registration)
    {
        bool Response = false;
        if(TypeMessage == ETypeOfMessage::Login)
        {
            Response = QRepository::getInstance().CheckAuth(SAuthorizationData(data));
        }
        else
        {
            Response = QRepository::getInstance().RegisterNewAccount(SAuthorizationData(data));
        }

        SBaseMessageData Result("", ETypeOfMessage::AuthResponse, Response);
        sender->sendMessage(Result);
        return;
    }


    for (Client *client : m_clients)
    {
        if (client == sender) continue;

        //client->sendMessage(data);
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
