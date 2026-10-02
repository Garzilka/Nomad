#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>
#include <QList>
#include <QQueue>
#include <QObject>
#include "Core/Data/CoreData.h"
#include "Core/Client/client.h"
#include "Core/DataBase/repository.h"


class Channel : public QObject
{
    Q_OBJECT
private:
    QString m_guid;
    QList<Client*> m_activeSubscribers;
    QQueue<SMessageData> m_historyCache; // Последние 100 сообщений в памяти

public:
    explicit Channel(QString guid, QObject *parent = nullptr)
        : QObject(parent), m_guid(guid)
    {
        m_historyCache.fromList(QRepository::getInstance().loadHistoryFromDB(m_guid, 50, 0));
    }

    void subscribe(Client* client)
    {
        if (m_activeSubscribers.contains(client)) return;

        m_activeSubscribers.append(client);
    }

    void unsubscribe(Client* client)
    {
        m_activeSubscribers.removeAll(client);
    }

    // Обработка нового сообщения
    void newMessage(const SMessageData& msg)
    {
        m_historyCache.enqueue(msg);
        if (m_historyCache.size() > 100)
        {
            m_historyCache.dequeue();
        }

        for (Client* client : m_activeSubscribers)
        {
            client->sendMessage(msg);
        }

        QRepository::getInstance().saveMessage(msg);
    }

    QList<SMessageData> GetHistory(int amount)
    {
        return m_historyCache.toList();
    }
};


#endif // CHANNEL_H
