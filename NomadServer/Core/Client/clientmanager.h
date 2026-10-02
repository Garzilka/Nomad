#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include "Core/Client/client.h"

#include "Core/Data/CoreData.h"
#include "Core/DataBase/repository.h"

class QClientManager : public QObject
{
    Q_OBJECT

    QHash<QString, QSharedPointer<Client>> m_activeCliens;

public:
    QClientManager(QObject *parent = nullptr) : QObject(parent) {};

    QClientManager& operator=(const QClientManager&) = delete;
    QClientManager(const QClientManager&) = delete;
    static QClientManager& getInstance()
    {
        static QClientManager instance(nullptr);
        return instance;
    };

    void newClient(Client* client, const QJsonObject& data, ETypeOfMessage& TypeMessage)
    {
        QString UUID = "";
        SAuthorizationData lData = SAuthorizationData(data);
        if(TypeMessage == ETypeOfMessage::Login)
        {
            UUID = QRepository::getInstance().CheckAuth(lData);
        }
        else
        {
            UUID = QRepository::getInstance().RegisterNewAccount(lData);
        }
        client->setUUID(UUID);
        bool Response = (!UUID.isEmpty());
        if(Response)
        {
            m_activeCliens.insert(UUID, QSharedPointer<Client>(client));
        }
        SAuthResponse Result(UUID, ETypeOfMessage::AuthResponse);
        client->sendMessage(Result);

        if (Response && TypeMessage == ETypeOfMessage::Login)
        {
            // Отправляем подтвержденных друзей
            handleGetFriendsList(client);

            // Отправляем активные заявки
            sendPendingRequests(client);
        }
    }

    QSharedPointer<Client> findOnlineClient(QString UUID)
    {
        return m_activeCliens[UUID];
    }
    void sendPendingRequests(Client* client)
    {
        QString myGUID = client->getGUID();

        // 1. Получаем все отношения из БД
        QList<SFriendInfo> fullList = QRepository::getInstance().getFriendsList(myGUID);
        QList<SFriendInfo> pendingList;

        // 2. Оставляем только заявки (0 - PendingSent, 1 - PendingReceived)
        for (int i = 0; i < fullList.size(); ++i)
        {
            if (fullList[i].RelationStatus == 0 || fullList[i].RelationStatus == 1)
            {
                // Параллельно актуализируем онлайн-статус
                if (m_activeCliens.contains(fullList[i].GUID))
                {
                    fullList[i].IsOnline = true;
                }
                else
                {
                    fullList[i].IsOnline = false;
                }
                pendingList.append(fullList[i]);
            }
        }

        // 3. Формируем пакет.
        SPendingRequestsData response;
        response.Response = true;
        response.Requests = pendingList;

        client->sendMessage(response);
        qInfo() << "CLIENT MANAGER | Пользователю" << myGUID << "отправлено заявок в друзья:" << pendingList.size();
    }

    void handleFriendRequest(Client* senderClient, const QJsonObject& data)
    {
        SFriendRequestData inData = SFriendRequestData(data);
        // 1. Вызываем логику БД
        // Передаем UUID отправителя (берем из объекта Client, чтобы избежать подмены) и ник цели
        auto dbResult = QRepository::getInstance().sendFriendRequest(senderClient->getGUID(), inData.TargetUsername);

        bool isSuccess = dbResult.first;
        QString targetGUID = dbResult.second;

        // 2. Отвечаем отправителю, успешна ли операция
        SFriendRequestData responseToSender(senderClient->getGUID(), inData.TargetUsername, targetGUID);
        responseToSender.Response = isSuccess; // true - отправлено, false - пользователь не найден/уже в друзьях
        senderClient->sendMessage(responseToSender);

        // 3. Real-time уведомление для получателя
        if (isSuccess && !targetGUID.isEmpty())
        {
            // Ищем, подключен ли получатель к нашему серверу прямо сейчас
            QSharedPointer<Client> targetClient = findOnlineClient(targetGUID);
            if (targetClient)
            {
                // Получатель онлайн! Формируем для него уведомление.
                SFriendRequestData notificationToTarget(senderClient->getGUID(), "");
                notificationToTarget.Response = true; // Сигнализирует, что это валидное событие

                targetClient->sendMessage(notificationToTarget);
            }
        }
    }

    void handleFriendResponseProcessing(Client* myClient, const QJsonObject& data)
    {
        SFriendResponseData inData = SFriendResponseData(data);
        QString myGUID = myClient->getGUID();
        QString targetGUID = inData.TargetGUID;

        // 1. Выполняем операцию в базе данных
        bool isSuccess = QRepository::getInstance().handleFriendResponse(myGUID, targetGUID, inData.Action);

        // 2. Отвечаем самому инициатору действия (тому, кто нажал кнопку в UI)
        SFriendResponseData responseToMe(targetGUID, inData.Action);
        responseToMe.Response = isSuccess;
        myClient->sendMessage(responseToMe);

        // 3. Если всё успешно и второй пользователь сейчас онлайн — уведомляем его в реальном времени
        if (isSuccess)
        {
            QSharedPointer<Client> targetClient = findOnlineClient(targetGUID);
            if (targetClient)
            {
                SFriendResponseData notificationToTarget(myGUID, inData.Action);
                notificationToTarget.Response = true; // Валидное событие обновления

                targetClient->sendMessage(notificationToTarget);
            }
        }
    }

    void handleGetFriendsList(Client* client)
    {
        QString myGUID = client->getGUID();

        // 1. Получаем базовый список отношений из БД
        QList<SFriendInfo> friendsList = QRepository::getInstance().getFriendsList(myGUID);

        // 2. Актуализируем статус присутствия в онлайне в реальном времени
        for (int i = 0; i < friendsList.size(); ++i)
        {
            // Если UUID друга есть в списке активных сессий сервера — он онлайн
            if (m_activeCliens.contains(friendsList[i].GUID))
            {
                friendsList[i].IsOnline = true;
            }
            else
            {
                friendsList[i].IsOnline = false;
            }
        }

        // 3. Упаковываем данные и отправляем клиенту
        SFriendsListData response;
        response.Response = true;
        response.Friends = friendsList;

        client->sendMessage(response);
    }

    void handleSearchUsers(Client* client, const QJsonObject& data)
    {
        SSearchUsersData inData(data);
        QString myGUID = client->getGUID();

        // 1. Ищем пользователей в БД по частичному совпадению
        QList<SFoundUserInfo> searchResults = QRepository::getInstance().searchUsersByUsername(myGUID, inData.SearchQuery);

        // 2. Формируем ответ клиенту
        SSearchUsersData response(inData.SearchQuery);
        response.Response = true;
        response.SearchQuery = inData.SearchQuery;
        response.Results = searchResults;

        // 3. Отправляем обратно запросившему клиенту
        client->sendMessage(response);
    }
};

#endif // CLIENTMANAGER_H
