#include "connection.h"
#include "Core/UIManager/uimanager.h"


#pragma region Template {
#pragma endregion }

#pragma region CORE {

QConnection::QConnection(QObject *parent) : QObject(parent)
{
    cryptolib = new QCryptoLib();
    m_socket = new QTcpSocket(this);

    if(DebugMode)
    {
        return;
    }
    connectToServer("127.0.0.1", 55);
    connect(m_socket, &QTcpSocket::connected, this, &QConnection::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &QConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &QConnection::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &QConnection::onErrorOccurred);
}

void QConnection::connectToServer(const QString &host, quint16 port)
{
    if(DebugMode)
    {
        setSecuritySuccessfully(true);
        return;
    }

    qDebug() << "Connect to" << host << ":" << port;
    if (m_socket->state() == QAbstractSocket::ConnectedState) return;

    m_socket->connectToHost(host, port);
}

void QConnection::onConnected()
{
    qDebug() << "Successful connection!";
}

void QConnection::onDisconnected()
{
    qDebug() << "Disconnect!";
    m_buffer.clear();
}

void QConnection::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qCritical() << "ERROR: Socket:" << m_socket->errorString();
}

#pragma endregion }

#pragma region SEND/READ {

bool QConnection::sendMessage(const SBaseMessageData &_Message)
{
    if(DebugMode)
    {
        return true;
    }

    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        QJsonDocument doc(_Message.ToJSON());
        QByteArray data = doc.toJson(QJsonDocument::Compact);

        if (cryptolib->hasSharedSecret())
        {
            data = cryptolib->encryptMessage(data);
        }

        quint32 size = data.size();
        QByteArray packet;
        packet.append(reinterpret_cast<const char*>(&size), 4);
        packet.append(data);

        m_socket->write(packet);
        m_socket->flush();
        return true;
    }
    else
    {
        qWarning() << "ERROR: Server not responding. Message was not sent.";
        return false;
    }
}

void QConnection::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    while (m_buffer.size() >= 4)
    {
        QJsonObject MainObject;
        int status = ReadCurrentPacket(MainObject);
        if(status == 2) return;
        if(status == 1) continue;

        SBaseMessageData Data(MainObject);

        switch (Data.TypeMessage)
        {
        case ETypeOfMessage::Crypto:
            Crypto(MainObject);
            break;
        case ETypeOfMessage::AuthResponse:
            if(Data.Response) AuthResponse(MainObject);
            break;
        case ETypeOfMessage::SearchUsers:
            if(Data.Response) SearchUsers(MainObject);
            break;
        case ETypeOfMessage::GetFriendsList:
            if(Data.Response) GetFriendsList(MainObject);
            break;
        case ETypeOfMessage::GetPendingRequests:
            if(Data.Response) GetPendingRequests(MainObject);
            break;
        case ETypeOfMessage::OpenPrivateChat:
            if(Data.Response) OpenPrivateChat(MainObject);
            break;
        case ETypeOfMessage::RequestHistoryChannel:
            if(Data.Response) RequestHistoryChannel(MainObject);
            break;
        case ETypeOfMessage::Message:
            NewMessage(MainObject);
            break;
        default:
            break;
        }
    }
}

#pragma endregion }

#pragma region REQUESTS {

void QConnection::authorization(const QString &Login, const QString &Password)
{
    if(!DebugMode)
    {
        SAuthorizationData sendData(ETypeOfMessage::Login, Login, "", Password);
        sendMessage(sendData);
        return;
    }
    setState(1);
}

void QConnection::registration(const QString &Login, const QString &Email, const QString &Password)
{
    if(!DebugMode)
    {
        SAuthorizationData sendData(ETypeOfMessage::Registration, Login, Email, Password);
        sendMessage(sendData);
        return;
    }
    setState(1);
}

void QConnection::searchFriend(const QString &UserName)
{
    if(!DebugMode)
    {
        SSearchUsersData sendData = SSearchUsersData(UserName);
        qDebug() << "Запрос поиска людей отправлен";
        sendMessage(sendData);
        return;
    }
}

void QConnection::requestFriend(const QString &GUID, const QString &UserName)
{
    if(!DebugMode)
    {
        SFriendRequestData sendData = SFriendRequestData(UUID, UserName, GUID);
        qDebug() << "Запрос дружбы отправлен";
        sendMessage(sendData);
        return;
    }
}

void QConnection::handleFriendAction(const QString &targetGUID, int action)
{
    if(!DebugMode)
    {
        SFriendResponseData sendData = SFriendResponseData(targetGUID, action);
        qDebug() << "Действие с заявкой:" << (action == 0 ? "Принять" : "Отклонить") << "для GUID:" << targetGUID;
        qDebug() << "Ответ дружбы отправлен";
        sendMessage(sendData);
        return;
    }
}

#pragma endregion }

#pragma region RESPONSE {

void QConnection::Crypto(QJsonObject &JSONObject)
{
    SCyptsKeysData CryptMessage(JSONObject);
    if (!(CryptMessage.Response && CryptMessage.TypeMessage == ETypeOfMessage::Crypto)) return;
    QByteArray peerKey = QByteArray::fromBase64(CryptMessage.PublicKey.toUtf8());

    if (!cryptolib->generateKeyPair())
    {
        qDebug() << "Не удалось сгенерировать пару ключей";
    }

    SCyptsKeysData ResponseCryptMessage(cryptolib->getPublicKeyDER().toBase64());
    sendMessage(ResponseCryptMessage);

    if (!cryptolib->computeSharedSecret(peerKey))
    {
        qDebug() << "Не удалось вычислить общий секрет";
    }

    qDebug() << "Шифрование установлено";
    setSecuritySuccessfully(true);
    emit OnKeyExchangeComplete();
}

void QConnection::AuthResponse(QJsonObject &JSONObject)
{
    UUID = JSONObject["UUID"].toString();
    setState(1);
}

void QConnection::SearchUsers(QJsonObject &JSONObject)
{
    SSearchUsersData searchData(JSONObject);
    QVariantList formattedResults;

    for (const SFoundUserInfo& user : searchData.Results)
    {
        QVariantMap userMap;
        userMap["guid"] = user.GUID;
        userMap["username"] = user.Username;
        userMap["displayName"] = user.DisplayName;
        userMap["avatarUrl"] = user.AvatarUrl;

        formattedResults.append(userMap);
    }

    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr)
    {
        friendMgr->setSearchResults(formattedResults);
    }
}

void QConnection::GetFriendsList(QJsonObject &JSONObject)
{
    SFriendsListData friendsData(JSONObject);
    QVariantList formattedFriends;

    for (const SFriendInfo& friendInfo : friendsData.Friends)
    {
        QVariantMap friendMap;
        friendMap["guid"] = friendInfo.GUID;
        friendMap["username"] = friendInfo.Username;
        friendMap["displayName"] = friendInfo.DisplayName;
        friendMap["avatarUrl"] = friendInfo.AvatarUrl;
        friendMap["globalStatus"] = friendInfo.GlobalStatus; // 'online', 'offline' и т.д.
        friendMap["isOnline"] = friendInfo.IsOnline;         // Живой статус присутствия на сервере

        formattedFriends.append(friendMap);
    }

    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr)
    {
        friendMgr->setFriendsList(formattedFriends);
    }
}

void QConnection::GetPendingRequests(QJsonObject &JSONObject)
{
    SPendingRequestsData pendingData(JSONObject);
    QVariantList formattedRequests;

    for (const SFriendInfo& req : pendingData.Requests)
    {
        QVariantMap reqMap;
        reqMap["guid"] = req.GUID;
        reqMap["username"] = req.Username;
        reqMap["displayName"] = req.DisplayName;
        reqMap["avatarUrl"] = req.AvatarUrl;
        reqMap["relationStatus"] = req.RelationStatus;

        formattedRequests.append(reqMap);
    }

    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr)
    {
        friendMgr->setPendingRequests(formattedRequests);
    }
}

void QConnection::OpenPrivateChat(QJsonObject &JSONObject)
{
    SOpenChatData openChatData(JSONObject);

    QChatManager* chatMgr = QUIManager::getInstance().GetChatManager();
    if (chatMgr)
    {
        chatMgr->setActiveRoomGUID(openChatData.RoomGUID);
    }
    QFriendManager* friendMgr = QUIManager::getInstance().GetFriendManager();
    if (friendMgr)
    {
        friendMgr->setState(1);
    }
    QUIManager::getInstance().setState(0);
}

void QConnection::RequestHistoryChannel(QJsonObject &JSONObject)
{
    SChannelHistoryData historyData(JSONObject);
    QVariantList formattedMessages;

    for (const SMessageData& msg : historyData.Messages)
    {
        QVariantMap msgMap;
        msgMap["sender"] = msg.SenderGUID;
        msgMap["messageText"] = msg.Message;
        msgMap["time"] = "12:00"; // Тут позже подставим реальный timestamp
        msgMap["avatarColor"] = "#5865f2";
        msgMap["msgType"] = msg.ContentType;

        formattedMessages.append(msgMap);
    }

    QChatManager* chatMgr = QUIManager::getInstance().GetChatManager();
    if (chatMgr)
    {
        chatMgr->setHistoryFromServer(historyData.RoomGUID, formattedMessages);
    }
}

void QConnection::NewMessage(QJsonObject &JSONObject)
{
    SMessageData msg(JSONObject);

    QVariantMap msgMap;
    msgMap["sender"] = msg.SenderGUID;
    msgMap["messageText"] = msg.Message;
    msgMap["time"] = QTime::currentTime().toString("hh:mm");
    msgMap["avatarColor"] = "#5865f2";
    msgMap["msgType"] = msg.ContentType;

    QChatManager* chatMgr = QUIManager::getInstance().GetChatManager();
    if (chatMgr)
    {
        chatMgr->addNewMessage(msg.RoomGUID, msgMap);
    }
}

#pragma endregion }

#pragma region TOOLS {

int QConnection::ReadCurrentPacket(QJsonObject& Result)
{
    quint32 size = *reinterpret_cast<const quint32*>(m_buffer.constData());

    if (size > 10 * 1024 * 1024)
    {
        qDebug() << "Слишком большой пакет, разрыв соединения";
        m_socket->disconnectFromHost();
        return 2;
    }

    if (m_buffer.size() < 4 + int(size)) return 1;

    QByteArray data = m_buffer.mid(4, size);
    m_buffer.remove(0, 4 + size);

    if (cryptolib->hasSharedSecret())
    {
        data = cryptolib->decryptMessage(data);
        if (data.isEmpty())
        {
            qDebug() << "Расшифровка не удалась";
            return 1;
        }
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Ошибка парсинга JSON:" << error.errorString();
        return 1;
    }
    Result = doc.object();
    return 0;
}

#pragma endregion }
