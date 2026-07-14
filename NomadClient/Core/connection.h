#ifndef CONNECTION_H
#define CONNECTION_H

#pragma once

#include <QObject>
#include <QTcpSocket>
#include "Core/Data/CoreData.h"




class QConnection : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *m_socket;

public:
    QConnection& operator=(const QConnection&) = delete;
    QConnection(const QConnection&) = delete;

    static QConnection& getInstance() {
        static QConnection instance(nullptr);
        instance.connectToServer("127.0.0.1", 55);
        return instance;
    };

    void connectToServer(const QString &host, quint16 port);
    bool sendMessage(SBaseMessageData* message);

Q_SIGNALS:
    void OnNewMessage(const QString Message);
    void OnAuthComplete();

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
