#ifndef CHANNELSMANAGER_H
#define CHANNELSMANAGER_H

#include <QHash>
#include <QObject>
#include <QSharedPointer>
#include "channel.h"
#include "Core/DataBase/repository.h"
#include "Core/Client/clientmanager.h"

class QChannelsManager : public QObject
{
    Q_OBJECT
private:

    QHash<QString, QSharedPointer<Channel>> m_activeChannels;


public:
    explicit QChannelsManager(QObject *parent = nullptr) : QObject(parent) {}

    QChannelsManager& operator=(const QChannelsManager&) = delete;
    QChannelsManager(const QChannelsManager&) = delete;
    static QChannelsManager& getInstance()
    {
        static QChannelsManager instance(nullptr);
        return instance;
    };

    void subscribeUserToChannel(const QString& roomGUID, Client* client)
    {
        if (roomGUID.isEmpty() || !client) return;

        // Если канала еще нет в памяти сервера — создаем его объект
        if (!m_activeChannels.contains(roomGUID))
        {
            if (QRepository::getInstance().isChannelExists(roomGUID))
            {
                QSharedPointer<Channel> newChannel(new Channel(roomGUID, this));
                m_activeChannels.insert(roomGUID, newChannel);
            }
            else
            {
                return; // Такого канала нет в БД
            }
        }

        // Подписываем сессию клиента на этот канал
        m_activeChannels[roomGUID]->subscribe(client);
        qInfo() << "CHANNELS MANAGER | Клиент" << client->getGUID() << "успешно подписан на канал:" << roomGUID;
    }

    void newMessage(Client* sender, const QJsonObject& data, const ETypeOfMessage& TypeOfMessage)
    {
        if(TypeOfMessage == ETypeOfMessage::Message)
        {
            SMessageData message(data);
            processClientMessage(sender, message);
        }

        if(TypeOfMessage == ETypeOfMessage::PrivateMessage)
        {
            SPrivateMessageData message(data);
            processPrivateClientMessage(sender, message);
        }
    }

    void handleOpenPrivateChat(Client* client, const QJsonObject& data)
    {
        SOpenChatData inData(data);
        QString senderGUID = client->getGUID();
        QString targetGUID = inData.TargetUserGUID;

        // 1. Получаем существующий или создаем новый канал в БД
        QString roomGUID = QRepository::getInstance().getOrCreatePrivateChannel(senderGUID, targetGUID);

        // 2. Отвечаем клиенту, передавая UUID комнаты
        SOpenChatData response;
        response.Response = !roomGUID.isEmpty();
        response.TargetUserGUID = targetGUID;
        response.RoomGUID = roomGUID;
        client->sendMessage(response);

        if (!roomGUID.isEmpty())
        {
            subscribeUserToChannel(roomGUID, client);
            QSharedPointer<Client> targetClient = QClientManager::getInstance().findOnlineClient(targetGUID);
            if (targetClient)
            {
                subscribeUserToChannel(roomGUID, targetClient.get());
            }
            QList<SMessageData> history = QRepository::getInstance().loadHistoryFromDB(roomGUID, 50, 0);

            SChannelHistoryData historyResponse;
            historyResponse.Response = true;
            historyResponse.RoomGUID = roomGUID;
            historyResponse.Messages = history;

            client->sendMessage(historyResponse);
        }
    }
private:
    // Основная точка входа при получении пакета ETypeOfMessage::Message
    void processClientMessage(Client* sender, const SMessageData& msg)
    {
        QString roomGUID = msg.RoomGUID;

        if (m_activeChannels.contains(roomGUID))
        {
            m_activeChannels[roomGUID]->newMessage(msg);
            return;
        }

        // В памяти нет, проверяем физическое наличие в БД
        if (QRepository::getInstance().isChannelExists(roomGUID))
        {
            // Канал легален! Создаем объект, инициализируем
            QSharedPointer<Channel> newChannel(new Channel(roomGUID, this));

            // Подгружаем последние 100 сообщений из БД в кэш канала
            // newChannel->loadHistoryFromDB();

            m_activeChannels.insert(roomGUID, newChannel);

            newChannel->subscribe(sender);
            newChannel->newMessage(msg);
            return;
        }

        qWarning() << "Попытка отправить сообщение в несуществующий канал:" << roomGUID;
    }

    void processPrivateClientMessage(Client* sender, const SPrivateMessageData& msg)
    {
        QString roomGUID = msg.RoomGUID;
        QString senderGUID = sender->getGUID(); // UUID того, кто пишет

        // RoomGUID ПУСТОЙ (Первое сообщение)
        QString targetGUID = msg.TargetUserGUID;
        QSharedPointer<Client> targetClient = QClientManager::getInstance().findOnlineClient(targetGUID);
        if(!targetClient) return;


        sender->sendMessage(msg);
        if (!roomGUID.isEmpty())
        {
            if (m_activeChannels.contains(roomGUID))
            {
                m_activeChannels[roomGUID]->newMessage(msg);
            }
            else if(QRepository::getInstance().isChannelExists(roomGUID))
            {
                QSharedPointer<Channel> newChannel(new Channel(roomGUID, this));
                m_activeChannels.insert(roomGUID, newChannel);
                newChannel->subscribe(sender);
                newChannel->subscribe(targetClient.get());
                newChannel->newMessage(msg);
            }
            return;
        }

        roomGUID = QRepository::getInstance().findPrivateChannel(senderGUID, targetGUID);

        if (roomGUID.isEmpty())
        {
            roomGUID = QUuid::createUuid().toString(); // Генерируем новый UUID канала
            QRepository::getInstance().createPrivateChannel(roomGUID, senderGUID, targetGUID);
        }

        if (!m_activeChannels.contains(roomGUID))
        {
            QSharedPointer<Channel> newChannel(new Channel(roomGUID, this));
            m_activeChannels.insert(roomGUID, newChannel);
        }

        QRepository::getInstance().saveMessage(msg);

        SPrivateMessageData responseToA = msg;
        responseToA.RoomGUID = roomGUID;
        sender->sendMessage(responseToA);

        m_activeChannels[roomGUID]->subscribe(sender);
        m_activeChannels[roomGUID]->subscribe(targetClient.get());
        m_activeChannels[roomGUID]->newMessage(msg);
    }
};


#endif // CHANNELSMANAGER_H
