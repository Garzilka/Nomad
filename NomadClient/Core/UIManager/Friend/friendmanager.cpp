#include "friendmanager.h"
#include "Core/connection.h"

void QFriendManager::handleFriendAction(const QString &targetGUID, int action)
{
    qDebug() << "FriendsManager | Действие с заявкой:" << (action == 0 ? "Принять" : "Отклонить") << "для GUID:" << targetGUID;

    for (int i = 0; i < m_pendingRequests.size(); ++i)
    {
        QVariantMap userMap = m_pendingRequests[i].toMap();
        if (userMap["guid"].toString() == targetGUID)
        {
            m_pendingRequests.removeAt(i);
            qDebug() << "FriendsManager | Карточка пользователя локально удалена из UI ожидания";
            emit pendingRequestsChanged();
            break;
        }
    }

    // 2. Отправляем сетевой пакет на сервер для фиксации изменений в PostgreSQL
    QConnection::getInstance().handleFriendAction(targetGUID, action);
}
