#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QTcpSocket *socket, QObject *parent = nullptr);
    ~Client();
    void sendMessage(const QByteArray &data);
    QString peerAddress() const;

signals:
    void messageReceived(Client *sender, const QByteArray &data);
    void disconnected(Client *client);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket *m_socket;
};

#endif // CLIENT_H
