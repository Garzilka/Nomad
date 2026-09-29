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
    Crypto = 5
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

struct SMessageData : public SBaseMessageData
{
    QString RoomGUID;
    QString Message;

    SMessageData(SBaseMessageData BaseData, QString _RoomGUID, QString _Message)
        : SBaseMessageData(BaseData), RoomGUID(_RoomGUID), Message(_Message) {};

    SMessageData(ETypeOfMessage _TypeMessage, QString _RoomGUID, QString _Message)
        : SBaseMessageData(_TypeMessage), RoomGUID(_RoomGUID), Message(_Message) {};

    SMessageData(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
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
