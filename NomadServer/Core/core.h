#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "Core/Client/client.h"


class QCore : public QObject {
    Q_OBJECT
public:
    explicit QCore(QObject *parent = nullptr);
    bool startServer(quint16 port);

private slots:
    void onNewConnection();
    void onReadyRead(Client *sender, const QByteArray &data);
    void onClientDisconnected(Client *client);

private:
    QTcpServer *m_tcpServer;
    QList<Client*> m_clients; // Список всех подключенных клиентов
};

#endif // CORE_H
