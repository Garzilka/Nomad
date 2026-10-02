#include "repository.h"
#include "Core/Lib/cryptolib.h"
#include "Core/DataBase/Rules/userscheckrule.h"
#include "Core/DataBase/Rules/checkdatabaserule.h"
#include "Core/DataBase/Rules/CheckChannelsRule.h"
#include "Core/DataBase/Rules/CheckChannelMembersRule.h"
#include "Core/DataBase/Rules/CheckMessagesRule.h"
#include "Core/DataBase/Rules/CheckFriendsRule.h"

QRepository::QRepository(QObject *parent) : QObject(parent)
{

}

bool QRepository::Run()
{
    CurrentRule = new CheckDataBaseRule();
    if(!CurrentRule) return false;

    if(!CurrentRule->CheckTable(db))
    {
        return false;
    }
    delete CurrentRule;

    db.setHostName("127.0.0.1");
    db.setDatabaseName("Nomad");
    db.setUserName("postgres");
    db.setPassword("12345678");
    db.setPort(5432);

    if (!db.open())
{
        qCritical() << "REPOSITORY ERROR| Final PostgreSQL connect error to 'Nomad':" << db.lastError().text();
        return false;
    }
    QVector<checkDBrule*> rules;

    rules.append(new UsersCheckRule());
    rules.append(new CheckChannelsRule());
    rules.append(new CheckChannelMembersRule());
    rules.append(new CheckMessagesRule());
    rules.append(new CheckFriendsRule());

    bool Result = true;
    for (checkDBrule* rule : rules)
    {
        if(!Result)
        {
            delete rule;
            continue;
        }

        Result &= rule->CheckTable(db);
        if (!Result)
        {
            qCritical() << "Критическая ошибка инициализации таблиц!";
        }
        delete rule;
    }

    if(Result)
    {
        qDebug() << "REPOSITORY|PostgreSQL: Connection succesfully!";
    }
    return Result;
}

QPair<bool, QString> QRepository::sendFriendRequest(const QString& senderGUID, const QString& targetUsername)
{
    QPair<bool, QString> result(false, "");

    if (!db.isOpen())
    {
        qCritical() << "БД не открыта в sendFriendRequest";
        return result;
    }

    QSqlQuery query(db);

    // 1. Ищем ID получателя по его username
    query.prepare("SELECT id FROM users WHERE username = :username;");
    query.bindValue(":username", targetUsername);
    if (!query.exec() || !query.next())
    {
        qWarning() << "Пользователь не найден:" << targetUsername;
        return result; // Пользователь с таким ником не существует
    }

    QString targetGUID = query.value(0).toString();
    result.second = targetGUID; // Сохраняем UUID для дальнейшего уведомления в сети

    // 2. Проверка: нельзя добавить самого себя
    if (senderGUID == targetGUID)
    {
        qWarning() << "Пользователь пытается добавить в друзья самого себя";
        return result;
    }

    // 3. Проверяем, нет ли уже каких-либо отношений (заявки, дружбы или блокировки)
    QString checkSql =
        "SELECT user_id, friend_id, status FROM friends "
        "WHERE (user_id = :sender_id AND friend_id = :target_id) "
        "   OR (user_id = :target_id AND friend_id = :sender_id);";

    query.prepare(checkSql);
    query.bindValue(":sender_id", senderGUID);
    query.bindValue(":target_id", targetGUID);

    if (!query.exec())
    {
        qCritical() << "Ошибка проверки существующих отношений:" << query.lastError().text();
        return result;
    }

    if (query.next())
    {
        QString relUser = query.value("user_id").toString();
        QString status = query.value("status").toString();

        if (status == "blocked")
        {
            qWarning() << "Отношения заблокированы. Заявка невозможна.";
            return result;
        }
        if (status == "accepted")
        {
            qWarning() << "Пользователи уже друзья.";
            return result;
        }
        if (status == "pending")
        {
            // Если заявку отправил НАМ этот же пользователь (relUser == targetGUID),
            // то вместо новой заявки нужно вызывать метод "Принять дружбу".
            // Но в рамках "отправки" мы просто вернем false, так как запись уже есть.
            qWarning() << "Заявка уже существует и ожидает подтверждения.";
            return result;
        }
    }

    // 4. Если проверок нет — создаем входящую заявку от sender к target
    query.prepare("INSERT INTO friends (user_id, friend_id, status) VALUES (:sender_id, :target_id, 'pending');");
    query.bindValue(":sender_id", senderGUID);
    query.bindValue(":target_id", targetGUID);

    if (!query.exec())
    {
        qCritical() << "Не удалось вставить запись заявки в друзья:" << query.lastError().text();
        return result;
    }

    result.first = true; // Успешно сохранено в БД
    return result;
}

bool QRepository::handleFriendResponse(const QString& myGUID, const QString& targetGUID, int action)
{
    if (!db.isOpen()) {
        qCritical() << "БД не открыта в handleFriendResponse";
        return false;
    }

    QSqlQuery query(db);

    if (action == 0) // Accept — Принять заявку
    {
        // Заявку отправил targetGUID, а мы (myGUID) её принимаем.
        // Поэтому ищем строку, где user_id = targetGUID AND friend_id = myGUID
        QString sql =
            "UPDATE friends SET status = 'accepted' "
            "WHERE user_id = :target_id AND friend_id = :my_id AND status = 'pending';";

        query.prepare(sql);
        query.bindValue(":target_id", targetGUID);
        query.bindValue(":my_id", myGUID);

        if (!query.exec())
        {
            qCritical() << "Ошибка при принятии заявки в друзья:" << query.lastError().text();
            return false;
        }

        // Проверяем, обновилась ли хоть одна строка (защита от фейковых запросов)
        return query.numRowsAffected() > 0;
    }
    else if (action == 1) // Reject / Cancel — Отклонить заявку или удалить из друзей
    {
        QString sql =
            "DELETE FROM friends "
            "WHERE (user_id = :my_id AND friend_id = :target_id) "
            "   OR (user_id = :target_id AND friend_id = :my_id);";

        query.prepare(sql);
        query.bindValue(":my_id", myGUID);
        query.bindValue(":target_id", targetGUID);

        if (!query.exec())
        {
            qCritical() << "Ошибка при удалении/отклонении заявки:" << query.lastError().text();
            return false;
        }

        return query.numRowsAffected() > 0;
    }

    return false;
}
QList<SFriendInfo> QRepository::getFriendsList(const QString& myGUID)
{
    QList<SFriendInfo> list;

    if (!db.isOpen()) {
        qCritical() << "БД не открыта в getFriendsList";
        return list;
    }

    QSqlQuery query(db);

    // Запрос выбирает связанных пользователей и вычисляет статус отношений (RelationStatus)
    // 0 - Наша исходящая заявка (PendingSent)
    // 1 - Входящая заявка к нам (PendingReceived)
    // 2 - Друзья (Friends)
    // 3 - Заблокирован нами (Blocked)
    QString sql = R"(
        SELECT
            u.id AS friend_id,
            u.username,
            u.display_name,
            u.avatar_url,
            u.status AS global_status,
            CASE
                WHEN f.status = 'accepted' THEN 2
                WHEN f.status = 'blocked' AND f.user_id = :my_id THEN 3
                WHEN f.status = 'pending' AND f.user_id = :my_id THEN 0
                WHEN f.status = 'pending' AND f.friend_id = :my_id THEN 1
                ELSE -1
            END AS relation_type
        FROM friends f
        JOIN users u ON (
            (f.user_id = :my_id AND u.id = f.friend_id) OR
            (f.friend_id = :my_id AND u.id = f.user_id)
        )
        WHERE (f.user_id = :my_id OR f.friend_id = :my_id)
          -- Исключаем строки, где нас заблокировал другой пользователь (в Discord их не видно)
          AND NOT (f.status = 'blocked' AND f.friend_id = :my_id);
    )";

    query.prepare(sql);
    query.bindValue(":my_id", myGUID);

    if (!query.exec())
    {
        qCritical() << "Ошибка получения списка друзей из БД:" << query.lastError().text();
        return list;
    }

    while (query.next())
    {
        int relationType = query.value("relation_type").toInt();
        if (relationType == -1) continue;

        SFriendInfo info;
        info.GUID = query.value("friend_id").toString();
        info.Username = query.value("username").toString();
        info.DisplayName = query.value("display_name").toString();
        info.AvatarUrl = query.value("avatar_url").toString();
        info.GlobalStatus = query.value("global_status").toString();
        info.RelationStatus = relationType;
        info.IsOnline = false;

        list.append(info);
    }

    return list;
}
QList<SFoundUserInfo> QRepository::searchUsersByUsername(const QString& myGUID, const QString& searchQuery)
{
    QList<SFoundUserInfo> results;

    if (searchQuery.trimmed().isEmpty())
    {
        return results;
    }

    if (!db.isOpen())
    {
        qCritical() << "БД не открыта в searchUsersByUsername";
        return results;
    }

    QSqlQuery query(db);

    QString sql =
        "SELECT id, username, display_name, avatar_url "
        "FROM users "
        "WHERE username ILIKE '%' || :search_query || '%' "
        "  AND id <> :my_id "
        "LIMIT 20;";

    query.prepare(sql);

    query.bindValue(":search_query", searchQuery.trimmed());
    query.bindValue(":my_id", myGUID);

    if (!query.exec()) {
        qCritical() << "Ошибка поиска пользователей по имени:" << query.lastError().text();
        return results;
    }

    while (query.next())
    {
        SFoundUserInfo info;
        info.GUID = query.value("id").toString();
        info.Username = query.value("username").toString();
        info.DisplayName = query.value("display_name").toString();
        info.AvatarUrl = query.value("avatar_url").toString();

        results.append(info);
    }

    return results;
}
QString QRepository::findPrivateChannel(const QString &senderGUID, const QString &targetGUID)
{
    if (!db.isOpen())
    {
        qCritical() << "БД не открыта в findPrivateChannel";
        return QString();
    }

    QSqlQuery query(db);

    QString sql =
        "SELECT cm.channel_id "
        "FROM channel_members cm "
        "JOIN channels c ON cm.channel_id = c.id "
        "WHERE c.type = 1 AND (cm.user_id = :sender_id OR cm.user_id = :target_id) "
        "GROUP BY cm.channel_id "
        "HAVING count(cm.user_id) = 2;";

    query.prepare(sql);
    query.bindValue(":sender_id", senderGUID);
    query.bindValue(":target_id", targetGUID);

    if (!query.exec())
    {
        qCritical() << "Ошибка выполнения findPrivateChannel:" << query.lastError().text();
        return QString();
    }

    // Если запись найдена, возвращаем UUID канала
    if (query.next())
    {
        return query.value(0).toString();
    }

    // Если переписки еще никогда не было, возвращаем пустую строку
    return QString();
}

QString QRepository::getOrCreatePrivateChannel(const QString &senderGUID, const QString &targetGUID)
{
    if (!db.isOpen()) {
        qCritical() << "REPOSITORY | БД не открыта в getOrCreatePrivateChannel";
        return QString();
    }

    QSqlQuery query(db);

    // Точный запрос: ищем channel_id, где пересекаются ровно эти два пользователя
    QString sql = R"(
        SELECT c.id
        FROM channels c
        JOIN channel_members cm1 ON c.id = cm1.channel_id
        JOIN channel_members cm2 ON c.id = cm2.channel_id
        WHERE c.type = 1
          AND cm1.user_id = :sender_id
          AND cm2.user_id = :target_id;
    )";

    query.prepare(sql);
    query.bindValue(":sender_id", senderGUID);
    query.bindValue(":target_id", targetGUID);

    if (query.exec() && query.next()) {
        QString existingRoomGUID = query.value(0).toString();
        qInfo() << "REPOSITORY | Найдена существующая DM-комната:" << existingRoomGUID;
        return existingRoomGUID;
    }

    // Если комнаты нет — генерируем новый UUID и создаем её
    // Генерируем UUID средствами Qt (QUuid) для вставки
    QString newRoomGUID = QUuid::createUuid().toString(QUuid::WithoutBraces);

    if (createPrivateChannel(newRoomGUID, senderGUID, targetGUID)) {
        qInfo() << "REPOSITORY | Создана новая DM-комната:" << newRoomGUID;
        return newRoomGUID;
    }

    return QString();
}
bool QRepository::createPrivateChannel(const QString &roomGUID, const QString &senderGUID, const QString &targetGUID)
{
    if (!db.isOpen())
    {
        qCritical() << "БД не открыта в createPrivateChannel";
        return false;
    }
    if (!db.transaction())
    {
        qCritical() << "Не удалось запустить транзакцию для создания канала";
        return false;
    }
    QSqlQuery query(db);

    query.prepare("INSERT INTO channels (id, type) VALUES (:room_id, 1);");
    query.bindValue(":room_id", roomGUID);

    if (!query.exec())
    {
        qCritical() << "Ошибка добавления в channels:" << query.lastError().text();
        db.rollback(); // Откатываем изменения
        return false;
    }

    query.prepare("INSERT INTO channel_members (channel_id, user_id) VALUES (:room_id, :user_id);");
    query.bindValue(":room_id", roomGUID);
    query.bindValue(":user_id", senderGUID);

    if (!query.exec())
    {
        qCritical() << "Ошибка добавления отправителя в channel_members:" << query.lastError().text();
        db.rollback();
        return false;
    }

    query.bindValue(":room_id", roomGUID);
    query.bindValue(":user_id", targetGUID);

    if (!query.exec())
    {
        qCritical() << "Ошибка добавления получателя в channel_members:" << query.lastError().text();
        db.rollback();
        return false;
    }

    return db.commit();
}

bool QRepository::saveMessage(const SMessageData& msg)
{
    if (!db.isOpen())
    {
        qCritical() << "БД не открыта в saveMessage";
        return false;
    }
    QSqlQuery query(db);

    // Поля id и created_at сгенерируются на стороне PostgreSQL автоматически (DEFAULT)
    QString sql =
        "INSERT INTO messages (channel_id, sender_id, content_type, content) "
        "VALUES (:channel_id, :sender_id, :content_type, :content);";

    query.prepare(sql);
    query.bindValue(":channel_id", msg.RoomGUID);
    query.bindValue(":sender_id", msg.SenderGUID);
    query.bindValue(":content_type", msg.ContentType); // 0 = Text, 1 = Img и т.д.
    query.bindValue(":content", msg.Message);

    if (!query.exec())
    {
        qCritical() << "Ошибка сохранения сообщения в БД:" << query.lastError().text();
        return false;
    }

    return true;
}

QList<SMessageData> QRepository::loadHistoryFromDB(const QString &roomGUID, int limit, int offset)
{
    QList<SMessageData> history;

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qCritical() << "БД не открыта в loadHistoryFromDB";
        return history;
    }

    QSqlQuery query(db);

    QString sql =
        "SELECT id, sender_id, content_type, content, created_at "
        "FROM messages "
        "WHERE channel_id = :channel_id "
        "ORDER BY created_at DESC "
        "LIMIT :limit OFFSET :offset;";

    query.prepare(sql);
    query.bindValue(":channel_id", roomGUID);
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (!query.exec()) {
        qCritical() << "Ошибка загрузки истории из БД:" << query.lastError().text();
        return history;
    }

    // Считываем данные из БД
    while (query.next())
    {
        QString senderGUID = query.value("sender_id").toString();
        int contentType = query.value("content_type").toInt();
        QString content = query.value("content").toString();

        QDateTime dateTime = query.value("created_at").toDateTime();
        QString timeStr = dateTime.time().toString("hh:mm");

        SMessageData msg(
            roomGUID,
            senderGUID,
            contentType,
            content
            );

        history.append(msg);
    }

    std::reverse(history.begin(), history.end());

    return history;
}

bool QRepository::isChannelExists(const QString &roomGUID)
{
    if (roomGUID.isEmpty())
    {
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qCritical() << "БД не открыта в isChannelExists";
        return false;
    }

    QSqlQuery query(db);

    QString sql = "SELECT 1 FROM channels WHERE id = :channel_id LIMIT 1;";

    query.prepare(sql);
    query.bindValue(":channel_id", roomGUID);

    if (!query.exec()) {
        qCritical() << "Ошибка при проверке существования канала:" << query.lastError().text();
        return false;
    }

    return query.next();
}

QString QRepository::RegisterNewAccount(const SAuthorizationData& Data)
{
    QString userGUID = "";
    if (HasAccount(Data.Login)) return userGUID;

    QByteArray SaltByte = QCryptoLib::generateSalt();
    QString Salt = QString::fromLatin1(SaltByte.toBase64());
    QString HashPass = QString::fromLatin1(QCryptoLib::hashPassword(Data.Password, SaltByte).toBase64());

    QString userEmail = Data.Email.isEmpty() ? QString("%1@example.com").arg(Data.Login) : Data.Email;

    QSqlQuery insertQuery(db);

    insertQuery.prepare("INSERT INTO users "
                        "(username, display_name, email, password_hash, salt) VALUES "
                        "(:username, :display_name, :email, :password_hash, :salt) "
                        "RETURNING id;");

    insertQuery.bindValue(":username", Data.Login);
    insertQuery.bindValue(":display_name", Data.Login);
    insertQuery.bindValue(":email", userEmail);
    insertQuery.bindValue(":password_hash", HashPass);
    insertQuery.bindValue(":salt", Salt);

    if (!insertQuery.exec())
    {
        qWarning() << "REPOSITORY ERROR | PostgreSQL INSERT error:" << insertQuery.lastError().text();
        return userGUID;
    }

    if (insertQuery.next())
    {
        userGUID = insertQuery.value(0).toString();
    }

    qInfo() << "REPOSITORY | Пользователь успешно зарегистрирован:" << Data.Login << "с UUID:" << userGUID;
    return userGUID;
}

bool QRepository::HasAccount(const QString& _Username)
{
    QSqlQuery query;
    query.prepare("SELECT EXISTS(SELECT 1 FROM users WHERE username = :username)");
    query.bindValue(":username", _Username);

    if (!query.exec())
    {
        qWarning() << "REPOSITORY ERROR|PostgreSQL check login error: " << query.lastError().text();
        return true;
    }

    if (query.next())
    {
        return query.value(0).toBool();
    }
    return true;
}

QString QRepository::CheckAuth(const SAuthorizationData& Data)
{
    QString userGUID = "";
    QSqlQuery query;

    query.prepare("SELECT id, password_hash, salt FROM users WHERE username = :username");
    query.bindValue(":username", Data.Login);

    if (!query.exec())
    {
        qWarning() << "REPOSITORY ERROR | PostgreSQL SELECT login error:" << query.lastError().text();
        return userGUID;
    }

    if (!query.next())
    {
        qWarning() << "REPOSITORY | Попытка входа: пользователь" << Data.Login << "не найден.";
        return userGUID;
    }

    QString dbHash = query.value("password_hash").toString();
    QString dbSaltBase64 = query.value("salt").toString();
    QByteArray saltByte = QByteArray::fromBase64(dbSaltBase64.toLatin1());

    QString computedHash = QString::fromLatin1(
        QCryptoLib::hashPassword(Data.Password, saltByte).toBase64()
        );

    if (computedHash != dbHash)
    {
        qWarning() << "REPOSITORY | Попытка входа: неверный пароль для пользователя" << Data.Login;
        return userGUID;
    }

    userGUID = query.value("id").toString();

    qInfo() << "REPOSITORY | Пользователь успешно авторизован:" << Data.Login << "GUID:" << userGUID;

    return userGUID;
}
