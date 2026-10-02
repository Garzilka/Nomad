#include "chatmanager.h"
#include "Core/connection.h"

void QChatManager::sendMessageFromUI(const QString &text)
{
    if (m_activeRoomGUID.isEmpty()) {
        qWarning() << "ChatManager | Попытка отправить сообщение в пустую комнату!";
        return;
    }

    qDebug() << "ChatManager | Отправка сообщения в комнату:" << m_activeRoomGUID << "Текст:" << text;

    // 1. Создаем локальную карточку сообщения для Optimistic UI (чтобы отобразилось сразу)
    /*QVariantMap localMsg;
    localMsg["sender"] = QConnection::getInstance().getUUID();
    localMsg["messageText"] = text;
    localMsg["time"] = QTime::currentTime().toString("hh:mm");
    localMsg["avatarColor"] = "#5865f2";
    localMsg["msgType"] = 0;

    // Добавляем сообщение в локальный кэш и на экран
    addNewMessage(m_activeRoomGUID, localMsg);*/

    // 2. Отправка сетевого пакета через QConnection
    SMessageData msg = SMessageData(m_activeRoomGUID, QConnection::getInstance().getUUID(), 0, text);
    QConnection::getInstance().sendMessage(msg);
}

void QChatManager::openPrivateChat(const QString &targetUserGUID)
{
    if (targetUserGUID.isEmpty())
    {
        qWarning() << "ChatManager | Попытка открыть чат с пустым GUID пользователя!";
        return;
    }

    qDebug() << "ChatManager | Запрос на открытие чата с пользователем:" << targetUserGUID;

    SOpenChatData msg = SOpenChatData();
    msg.TargetUserGUID = targetUserGUID;
    QConnection::getInstance().sendMessage(msg);
}
