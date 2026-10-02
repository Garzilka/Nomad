#ifndef COREDATA_H
#define COREDATA_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

enum ETypeOfMessage
{
    Login = 0,
    Registration = 1,
    AuthResponse = 2,
    Message = 3,
    Disconnect = 4,
    Crypto = 5,
    PrivateMessage = 6,
    RequestHistoryChannel = 7,
    RequestFriend = 8,
    ResponseFriend = 9,
    GetFriendsList = 10,
    SearchUsers = 11,
    GetPendingRequests = 12,
    OpenPrivateChat = 13
};
enum EFriendStatus
{
    PendingSent = 0,
    PendingReceived = 1,
    Friends = 2,
    Blocked = 3
};
struct SBaseMessageData
{
    ETypeOfMessage TypeMessage;
    bool Response = false;

    SBaseMessageData(ETypeOfMessage _TypeMessage, bool _Response = true)
        : TypeMessage(_TypeMessage), Response(_Response)
    {};

    SBaseMessageData(QJsonObject InJSON)
    {
        Response = InJSON["Response"].toBool();
        TypeMessage = static_cast<ETypeOfMessage>(InJSON["TypeMessage"].toInt());
    };

    virtual QJsonObject ToJSON() const
    {
        QJsonObject messageObj;
        messageObj["TypeMessage"] = static_cast<int>(TypeMessage);
        messageObj["Response"] = Response;
        return messageObj;
    }
};

struct SCyptsKeysData : public SBaseMessageData
{
    QString PublicKey;

    SCyptsKeysData(QString _PublicKey, ETypeOfMessage _TypeMessage = ETypeOfMessage::Crypto)
        : SBaseMessageData(_TypeMessage), PublicKey(_PublicKey) {};

    SCyptsKeysData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        PublicKey = InJSON["PublicKey"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["PublicKey"] = PublicKey;
        return messageObj;
    }
};

struct SAuthorizationData : public SBaseMessageData
{
    QString Login;
    QString Email;
    QString Password;

    SAuthorizationData(SBaseMessageData BaseData, QString _Login, QString _Email, QString _Password)
        : SBaseMessageData(BaseData), Login(_Login), Email(_Email), Password(_Password) {};

    SAuthorizationData(ETypeOfMessage _TypeMessage, QString _Login, QString _Email, QString _Password)
        : SBaseMessageData(_TypeMessage), Login(_Login), Email(_Email), Password(_Password) {};

    SAuthorizationData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Login = InJSON["Login"].toString();
        Password = InJSON["Password"].toString();
        Email = InJSON["Email"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["Login"] = Login;
        messageObj["Password"] = Password;
        messageObj["Email"] = Email;
        return messageObj;
    }
};

struct SAuthResponse : public SBaseMessageData
{
    QString UUID;

    SAuthResponse(QString _UUID, ETypeOfMessage _TypeMessage)
        : SBaseMessageData(_TypeMessage), UUID(_UUID) {};

    SAuthResponse(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        UUID = InJSON["UUID"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["UUID"] = UUID;
        return messageObj;
    }
};

enum ETypeChatMessage
{
    Text = 0,
    Img = 1,
    Sticker = 2,
    Faile = 3,
    InvitationServer = 4,
};

struct SMessageData : public SBaseMessageData
{
    QString RoomGUID;
    QString SenderGUID;
    int ContentType;
    QString Message;

    SMessageData(QString _RoomGUID, QString _SenderGUID, int _ContentType, QString _Message, ETypeOfMessage _TypeMessage = ETypeOfMessage::Message)
        : SBaseMessageData(_TypeMessage), RoomGUID(_RoomGUID), SenderGUID(_SenderGUID), ContentType(_ContentType), Message(_Message) {};

    SMessageData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        RoomGUID = InJSON["RoomGUID"].toString();
        SenderGUID = InJSON["SenderGUID"].toString();
        ContentType = InJSON["ContentType"].toInt();
        Message = InJSON["Message"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["RoomGUID"] = RoomGUID;
        messageObj["SenderGUID"] = SenderGUID;
        messageObj["ContentType"] = ContentType;
        messageObj["Message"] = Message;
        return messageObj;
    }
};

struct SPrivateMessageData : public SMessageData
{
    QString TargetUserGUID;

    SPrivateMessageData(ETypeOfMessage _TypeMessage, QString _RoomGUID, QString _SenderGUID, int _ContentType, QString _Message, QString _TargetUserGUID)
        : SMessageData(_RoomGUID, _SenderGUID, _ContentType, _Message, _TypeMessage), TargetUserGUID(_TargetUserGUID) {};

    SPrivateMessageData(const QJsonObject& InJSON) : SMessageData(InJSON)
    {
        TargetUserGUID = InJSON["TargetUserGUID"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SMessageData::ToJSON();
        messageObj["TargetUserGUID"] = TargetUserGUID;
        return messageObj;
    }
};

struct SFriendRequestData : public SBaseMessageData
{
    QString SenderGUID;
    QString TargetUsername;
    QString TargetGUID;

    SFriendRequestData(QString _SenderGUID, QString _TargetUsername, QString _TargetGUID = "", ETypeOfMessage _TypeMessage = ETypeOfMessage::RequestFriend)
        : SBaseMessageData(_TypeMessage), SenderGUID(_SenderGUID), TargetUsername(_TargetUsername), TargetGUID(_TargetGUID) {};

    SFriendRequestData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        SenderGUID = InJSON["SenderGUID"].toString();
        TargetUsername = InJSON["TargetUsername"].toString();
        TargetGUID = InJSON["TargetGUID"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SenderGUID"] = SenderGUID;
        messageObj["TargetUsername"] = TargetUsername;
        messageObj["TargetGUID"] = TargetGUID;
        return messageObj;
    }
};

struct SFriendResponseData : public SBaseMessageData
{
    QString TargetGUID;         // UUID пользователя, чью заявку мы обрабатываем
    bool Action = false;        // 0 - Принять, 1 - Отклонить/Удалить

    SFriendResponseData(QString _TargetGUID, int _Action, ETypeOfMessage _TypeMessage = ETypeOfMessage::ResponseFriend)
        : SBaseMessageData(_TypeMessage), TargetGUID(_TargetGUID), Action(_Action) {};

    SFriendResponseData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        TargetGUID = InJSON["TargetGUID"].toString();
        Action = InJSON["Action"].toBool();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["TargetGUID"] = TargetGUID;
        messageObj["Action"] = Action;
        return messageObj;
    }
};

struct SFriendInfo
{
    QString GUID;
    QString Username;
    QString DisplayName;
    QString AvatarUrl;
    QString GlobalStatus;
    int RelationStatus;
    bool IsOnline;

    QJsonObject ToJSON() const
    {
        QJsonObject obj;
        obj["GUID"] = GUID;
        obj["Username"] = Username;
        obj["DisplayName"] = DisplayName;
        obj["AvatarUrl"] = AvatarUrl;
        obj["GlobalStatus"] = GlobalStatus;
        obj["RelationStatus"] = RelationStatus;
        obj["IsOnline"] = IsOnline;
        return obj;
    }

    static SFriendInfo FromJSON(const QJsonObject& obj)
    {
        SFriendInfo info;
        info.GUID = obj["GUID"].toString();
        info.Username = obj["Username"].toString();
        info.DisplayName = obj["DisplayName"].toString();
        info.AvatarUrl = obj["AvatarUrl"].toString();
        info.GlobalStatus = obj["GlobalStatus"].toString();
        info.RelationStatus = obj["RelationStatus"].toInt();
        info.IsOnline = obj["IsOnline"].toBool();
        return info;
    }
};

// Контейнер для отправки всего списка клиенту
struct SFriendsListData : public SBaseMessageData
{
    QList<SFriendInfo> Friends;

    SFriendsListData(ETypeOfMessage _TypeMessage = ETypeOfMessage::GetFriendsList)
        : SBaseMessageData(_TypeMessage) {};

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        QJsonArray arr;
        for(const auto& friendInfo : Friends)
        {
            arr.append(friendInfo.ToJSON());
        }
        messageObj["Friends"] = arr;
        return messageObj;
    }
    SFriendsListData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Friends.clear(); // Очищаем список перед заполнением

        if (InJSON.contains("Friends") && InJSON["Friends"].isArray())
        {
            QJsonArray arr = InJSON["Friends"].toArray();
            for (const QJsonValue& value : arr)
            {
                if (value.isObject())
                {
                    // Восстанавливаем элемент SFriendInfo из JSON объекта
                    Friends.append(SFriendInfo::FromJSON(value.toObject()));
                }
            }
        }
    };
};

struct SFoundUserInfo
{
    QString GUID;
    QString Username;
    QString DisplayName;
    QString AvatarUrl;

    QJsonObject ToJSON() const
    {
        QJsonObject obj;
        obj["GUID"] = GUID;
        obj["Username"] = Username;
        obj["DisplayName"] = DisplayName;
        obj["AvatarUrl"] = AvatarUrl;
        return obj;
    }
    SFoundUserInfo() = default;
    SFoundUserInfo(const QJsonObject& obj)
    {
        GUID = obj["GUID"].toString();
        Username = obj["Username"].toString();
        DisplayName = obj["DisplayName"].toString();
        AvatarUrl = obj["AvatarUrl"].toString();
    }
};

// 3. Структура-контейнер запроса и ответа поиска
struct SSearchUsersData : public SBaseMessageData
{
    QString SearchQuery;           // Строка поиска (что ввел пользователь)
    QList<SFoundUserInfo> Results; // Массив найденных пользователей

    SSearchUsersData(QString _SearchQuery, ETypeOfMessage _TypeMessage = ETypeOfMessage::SearchUsers)
        : SBaseMessageData(_TypeMessage), SearchQuery(_SearchQuery) {};

    SSearchUsersData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        SearchQuery = InJSON["SearchQuery"].toString();

        // Очищаем список на всякий случай перед заполнением
        Results.clear();

        // Проверяем, содержит ли JSON массив результатов (он приходит от сервера)
        if (InJSON.contains("Results") && InJSON["Results"].isArray())
        {
            QJsonArray arr = InJSON["Results"].toArray();
            for (const QJsonValue& value : arr)
            {
                if (value.isObject())
                {
                    // Используем наш статический метод восстановления объекта
                    Results.append(SFoundUserInfo(value.toObject()));
                }
            }
        }
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SearchQuery"] = SearchQuery;

        QJsonArray arr;
        for(const auto& user : Results)
        {
            arr.append(user.ToJSON());
        }
        messageObj["Results"] = arr;
        return messageObj;
    }
};

struct SPendingRequestsData : public SBaseMessageData
{
    QList<SFriendInfo> Requests;

    SPendingRequestsData(ETypeOfMessage _TypeMessage = ETypeOfMessage::GetPendingRequests)
        : SBaseMessageData(_TypeMessage) {};

    SPendingRequestsData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        Requests.clear();
        if (InJSON.contains("Requests") && InJSON["Requests"].isArray())
        {
            QJsonArray arr = InJSON["Requests"].toArray();
            for (const QJsonValue& value : arr)
            {
                if (value.isObject())
                {
                    Requests.append(SFriendInfo::FromJSON(value.toObject()));
                }
            }
        }
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        QJsonArray arr;
        for(const auto& requestInfo : Requests)
        {
            arr.append(requestInfo.ToJSON());
        }
        messageObj["Requests"] = arr;
        return messageObj;
    }
};

struct SOpenChatData : public SBaseMessageData
{
    QString TargetUserGUID; // Кого кликнули в списке друзей (UUID)
    QString RoomGUID;       // Идентификатор комнаты (заполнит сервер)

    SOpenChatData(ETypeOfMessage _TypeMessage = ETypeOfMessage::OpenPrivateChat)
        : SBaseMessageData(_TypeMessage) {};

    SOpenChatData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        TargetUserGUID = InJSON["TargetUserGUID"].toString();
        RoomGUID = InJSON["RoomGUID"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["TargetUserGUID"] = TargetUserGUID;
        messageObj["RoomGUID"] = RoomGUID;
        return messageObj;
    }
};

struct SChannelHistoryData : public SBaseMessageData
{
    QString RoomGUID;
    QList<SMessageData> Messages;

    SChannelHistoryData(ETypeOfMessage _TypeMessage = ETypeOfMessage::RequestHistoryChannel)
        : SBaseMessageData(_TypeMessage) {};

    SChannelHistoryData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        RoomGUID = InJSON["RoomGUID"].toString();
        Messages.clear();

        if (InJSON.contains("Messages") && InJSON["Messages"].isArray())
        {
            QJsonArray arr = InJSON["Messages"].toArray();
            for (const QJsonValue& value : arr)
            {
                if (value.isObject())
                {
                    Messages.append(SMessageData(value.toObject()));
                }
            }
        }
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["RoomGUID"] = RoomGUID;

        QJsonArray arr;
        for(const auto& msg : Messages)
        {
            arr.append(msg.ToJSON());
        }
        messageObj["Messages"] = arr;
        return messageObj;
    }
};
#endif // COREDATA_H
