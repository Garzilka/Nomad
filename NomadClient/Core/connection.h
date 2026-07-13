#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>

class Connection : public QObject
{
    Q_OBJECT

public:
    explicit Connection(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    bool sendMessage(const QString &message);

Q_SIGNALS:
    void OnNewMessage(const QString Message);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;
};

#endif // CONNECTION_H
