#ifndef CONNECTION_H
#define CONNECTION_H

#pragma once

#include <QObject>
#include <QTcpSocket>
#include "Core/Data/CoreData.h"
#include "Core/Lib/cryptolib.h"




class QConnection : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *m_socket;

    Q_PROPERTY(int State READ getState WRITE setState NOTIFY StateChanged)
    Q_PROPERTY(bool securitySuccessfully READ getSecuritySuccessfully WRITE setSecuritySuccessfully NOTIFY securitySuccessfullyChanged)

    int State = 0; // 0 - Auth | 1 - WorkSpace | 2 - Settings
    bool SecuritySuccefully = false;
    QCryptoLib* cryptolib = nullptr;
    QByteArray m_buffer;

    bool DebugMode = false;
    QString UUID = "";

public:
    QConnection& operator=(const QConnection&) = delete;
    QConnection(const QConnection&) = delete;
    static QConnection& getInstance()
    {
        static QConnection instance(nullptr);
        return instance;
    };


    QString getUUID() { return UUID; };
    int getState() { return State; }
    bool getSecuritySuccessfully() { return SecuritySuccefully; }

    Q_INVOKABLE void setState(int NewNum)
    {
        if (State != NewNum)
        {
            State = NewNum;
            emit StateChanged();
        }
    }

    Q_INVOKABLE void setSecuritySuccessfully(bool NewState)
    {
        if (SecuritySuccefully != NewState)
        {
            SecuritySuccefully = NewState;
            emit securitySuccessfullyChanged();
        }
    }

    void connectToServer(const QString &host, quint16 port);
    bool sendMessage(const SBaseMessageData& message);


    /* ============== REQUEST ============== */
    Q_INVOKABLE void authorization(const QString &Login, const QString &Password);
    Q_INVOKABLE void registration(const QString &Login, const QString &Email, const QString &Password);
    Q_INVOKABLE void searchFriend(const QString &UserName);
    Q_INVOKABLE void requestFriend(const QString &GUID, const QString &UserName);
    Q_INVOKABLE void handleFriendAction(const QString& targetGUID, int action);
    /* ====================================== */


signals:
    void StateChanged();
    void securitySuccessfullyChanged();

Q_SIGNALS:

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:

    int ReadCurrentPacket(QJsonObject& Result);

    /* ============== RESPONSE ============== */
    void Crypto(QJsonObject& JSONObject);
    void AuthResponse(QJsonObject& JSONObject);
    void SearchUsers(QJsonObject& JSONObject);
    void GetFriendsList(QJsonObject& JSONObject);
    void GetPendingRequests(QJsonObject& JSONObject);
    void OpenPrivateChat(QJsonObject& JSONObject);
    void RequestHistoryChannel(QJsonObject& JSONObject);
    void NewMessage(QJsonObject& JSONObject);
    /* ====================================== */

    explicit QConnection(QObject *parent = nullptr);
    ~QConnection() { }

};

#endif // CONNECTION_H
