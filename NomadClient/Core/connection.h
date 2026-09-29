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

public:
    QConnection& operator=(const QConnection&) = delete;
    QConnection(const QConnection&) = delete;

    Q_INVOKABLE void setState(int NewNum)
    {
        if (State != NewNum)
        {
            State = NewNum;
            emit StateChanged();
        }
    }

    int getState()
    {
        return State;
    }

    Q_INVOKABLE void setSecuritySuccessfully(bool NewState)
    {
        if (SecuritySuccefully != NewState)
        {
            SecuritySuccefully = NewState;
            emit securitySuccessfullyChanged();
        }
    }

    bool getSecuritySuccessfully()
    {
        return SecuritySuccefully;
    }

    static QConnection& getInstance() {
        static QConnection instance(nullptr);
        instance.connectToServer("127.0.0.1", 55);
        return instance;
    };

    void connectToServer(const QString &host, quint16 port);
    bool sendMessage(const SBaseMessageData& message);

    Q_INVOKABLE void authorization(const QString &Login, const QString &Password);
    Q_INVOKABLE void registration(const QString &Login, const QString &Email, const QString &Password);

signals:
    void StateChanged();
    void securitySuccessfullyChanged();

Q_SIGNALS:
    void OnNewMessage(const QString Message);
    void OnAuthComplete();
    void OnKeyExchangeComplete();

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    explicit QConnection(QObject *parent = nullptr);
    ~QConnection() { }

};

#endif // CONNECTION_H
