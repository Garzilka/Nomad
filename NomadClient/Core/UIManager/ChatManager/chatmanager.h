#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QHash>
#include <QDebug>
#include "Core/Data/CoreData.h"

class QChatManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString activeRoomGUID READ getActiveRoomGUID WRITE setActiveRoomGUID NOTIFY activeRoomGUIDChanged)
    Q_PROPERTY(QVariantList currentMessages READ getCurrentMessages NOTIFY currentMessagesChanged)

    QString m_activeRoomGUID;
    QVariantList m_currentMessages;
    QHash<QString, QVariantList> m_chatCache;

public:
    QChatManager(QObject *parent = nullptr) : QObject(parent) {};

    void setActiveRoomGUID(const QString& guid)
    {
        if (m_activeRoomGUID != guid)
        {
            m_activeRoomGUID = guid;
            emit activeRoomGUIDChanged();

            // Проверяем, есть ли уже история для этой комнаты в нашем кэше
            if (m_chatCache.contains(m_activeRoomGUID))
            {
                m_currentMessages = m_chatCache[m_activeRoomGUID];
                qInfo() << "ChatManager | Комната переключена. Данные загружены из КЭША. Сообщений:" << m_currentMessages.size();
            }
            else
            {
                m_currentMessages.clear(); // Кэш пуст, ждем пока сервер пришлет историю
                qInfo() << "ChatManager | Комната переключена. Кэш пуст, ожидается загрузка по сети...";
            }
            emit currentMessagesChanged(); // Навечно обновляет ListView в QML
        }
    }
    void setHistoryFromServer(const QString& roomGUID, const QVariantList& messages)
    {
        // 1. Сохраняем в кэш
        m_chatCache[roomGUID] = messages;

        // 2. Если история пришла для комнаты, которая открыта прямо сейчас — выводим на экран
        if (m_activeRoomGUID == roomGUID)
        {
            m_currentMessages = messages;
            emit currentMessagesChanged();
        }
    }
    void addNewMessage(const QString& roomGUID, const QVariantMap& message)
    {
        // Добавляем в кэш конкретной комнаты
        m_chatCache[roomGUID].append(message);

        // Если это сообщение из текущего открытого чата — сразу пускаем на экран
        if (m_activeRoomGUID == roomGUID)
        {
            m_currentMessages.append(message);
            emit currentMessagesChanged();
        }
    }
    QVariantList getCurrentMessages() const { return m_currentMessages; }

    QString getActiveRoomGUID() const { return m_activeRoomGUID; }

    Q_INVOKABLE void sendMessageFromUI(const QString& text);
    Q_INVOKABLE void openPrivateChat(const QString& targetUserGUID);
signals:
    void historyLoaded(const QVariantList& messages);
    void newMessageReceived(const QVariantMap& message);
    void activeRoomGUIDChanged();
    void currentMessagesChanged();
};

#endif // CHATMANAGER_H
