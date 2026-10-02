#include "core.h"
#include "Core/DataBase/repository.h"
#include "Core/Client/clientmanager.h"
#include "Core/Channel/channelsmanager.h"

QCore::QCore(QObject *parent) : QObject(parent){}


bool QCore::startServer(quint16 port)
{
    if(!QRepository::getInstance().Run())
    {
        return false;
    }
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &QCore::onNewConnection);
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

void QCore::onReadyRead(Client *sender, QJsonObject& data, ETypeOfMessage& TypeMessage)
{
    if (!sender) return;

    if(TypeMessage == ETypeOfMessage::Login || TypeMessage == ETypeOfMessage::Registration)
    {
        QClientManager::getInstance().newClient(sender, data, TypeMessage);
        return;
    }

    if(TypeMessage == ETypeOfMessage::Message || TypeMessage == ETypeOfMessage::PrivateMessage)
    {
        QChannelsManager::getInstance().newMessage(sender, data, TypeMessage);
        return;
    }

    if(TypeMessage == ETypeOfMessage::RequestFriend)
    {
        QClientManager::getInstance().handleFriendRequest(sender, data);
        return;
    }

    if(TypeMessage == ETypeOfMessage::ResponseFriend)
    {
        QClientManager::getInstance().handleFriendResponseProcessing(sender, data);
        return;
    }

    if(TypeMessage == ETypeOfMessage::GetFriendsList)
    {
        QClientManager::getInstance().handleGetFriendsList(sender);
        return;
    }

    if(TypeMessage == ETypeOfMessage::SearchUsers)
    {
        QClientManager::getInstance().handleSearchUsers(sender, data);
        return;
    }
    if(TypeMessage == ETypeOfMessage::OpenPrivateChat)
    {
        QChannelsManager::getInstance().handleOpenPrivateChat(sender, data);
        return;
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
