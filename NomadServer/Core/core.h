#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>


class QCore : public QObject {
    Q_OBJECT
public:
    explicit QCore(QObject *parent = nullptr);
    bool startServer(quint16 port);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    QTcpServer *m_tcpServer;
    QList<QTcpSocket*> m_clientSockets; // Список всех подключенных клиентов
};

#endif // CORE_H
