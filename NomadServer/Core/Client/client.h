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

signals:
    void OnMessageReceived(Client *sender, QJsonObject& data);
    void disconnected(Client *client);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;
    QString Login;
    QString Nickname;
    QCryptoLib* cryptolib = nullptr;
};

#endif // CLIENT_H
