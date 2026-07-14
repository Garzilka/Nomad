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
    Disconnect = 4
};

struct SBaseMessageData
{
    QString PublicToken; //public token for another users, for can request public data: nickname, an avatar icon, etc.
    ETypeOfMessage TypeMessage;
    bool Response = false;

    SBaseMessageData(QString _Token, ETypeOfMessage _TypeMessage, bool _Response = true)
        : PublicToken(_Token),
        TypeMessage(_TypeMessage),
        Response(_Response)
    {};

    SBaseMessageData(QJsonObject InJSON)
    {
        PublicToken = InJSON["Token"].toString();
        Response = InJSON["Response"].toBool();
        TypeMessage = static_cast<ETypeOfMessage>(InJSON["TypeMessage"].toInt());
    };

    virtual QJsonObject ToJSON() const
    {
        QJsonObject messageObj;
        messageObj["TypeMessage"] = static_cast<int>(TypeMessage);
        messageObj["Response"] = Response;
        messageObj["Token"] = PublicToken;
        return messageObj;
    }
};

struct SAuthorizationData : public SBaseMessageData
{
    QString Login;
    QString Password;

    SAuthorizationData(SBaseMessageData BaseData, QString _Login, QString _Password)
        : SBaseMessageData(BaseData),
        Login(_Login),
        Password(_Password) {};

    SAuthorizationData(QString _Token, ETypeOfMessage _TypeMessage, QString _Login, QString _Password)
        : SBaseMessageData(_Token, _TypeMessage),
        Login(_Login),
        Password(_Password) {};

    SAuthorizationData(QJsonObject InJSON) : SBaseMessageData(InJSON)
    {
        Login = InJSON["Login"].toString();
        Password = InJSON["Password"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["Login"] = Login;
        messageObj["Password"] = Password;
        return messageObj;
    }
};

struct SMessageData : public SBaseMessageData
{
    QString RoomGUID;
    QString Message;

    SMessageData(SBaseMessageData BaseData, QString _RoomGUID, QString _Message)
        : SBaseMessageData(BaseData),
        RoomGUID(_RoomGUID),
        Message(_Message) {};

    SMessageData(QString _Token, ETypeOfMessage _TypeMessage, QString _RoomGUID, QString _Message)
        : SBaseMessageData(_Token, _TypeMessage),
        RoomGUID(_RoomGUID),
        Message(_Message) {};

    SMessageData(QJsonObject InJSON) : SBaseMessageData(InJSON)
    {
        RoomGUID = InJSON["RoomGUID"].toString();
        Message = InJSON["Message"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["RoomGUID"] = RoomGUID;
        messageObj["Message"] = Message;
        return messageObj;
    }
};

#endif // COREDATA_H
