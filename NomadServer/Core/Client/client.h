#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include "Core/Data/CoreData.h"
#include "Core/Lib/cryptolib.h"

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QTcpSocket *socket, QObject *parent = nullptr);
    ~Client();
    void sendMessage(const SBaseMessageData &_Message);
    QString peerAddress() const;
    void setUUID(QString UUID) {_UUID = UUID;}
    QString getGUID() { return _UUID; }
signals:
    void OnMessageReceived(Client *sender, QJsonObject& data, ETypeOfMessage& TypeMessage);
    void disconnected(Client *client);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;
    QString _UUID;
    QString Nickname;
    QCryptoLib* cryptolib = nullptr;
};

#endif // CLIENT_H
