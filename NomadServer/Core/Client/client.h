#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include "Core/Data/CoreData.h"

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QTcpSocket *socket, QObject *parent = nullptr);
    ~Client();
    void sendMessage(SBaseMessageData &data);
    QString peerAddress() const;

signals:
    void OnMessageReceived(Client *sender, QJsonObject& data);
    void disconnected(Client *client);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket *m_socket;
    QString Login;
    QString Nickname;
};

#endif // CLIENT_H
