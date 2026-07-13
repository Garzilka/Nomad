#include "core.h"


QCore::QCore(QObject *parent) : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &QCore::onNewConnection);
}


bool QCore::startServer(quint16 port)
{
    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        qCritical() << "Сервер не смог запуститься:" << m_tcpServer->errorString();
        return false;
    }
    qInfo() << "Сервер успешно запущен на порту" << port;
    return true;
}

void QCore::onNewConnection()
{
    // Получаем сокет для общения с новым клиентом
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();

    m_clientSockets.append(clientSocket);
    qInfo() << "Новое подключение от:" << clientSocket->peerAddress().toString();

    // Связываем сигналы сокета с обработчиками
    connect(clientSocket, &QTcpSocket::readyRead, this, &QCore::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &QCore::onClientDisconnected);
}

void QCore::onReadyRead()
{
    QTcpSocket *senderSocket = qobject_cast<QTcpSocket*>(sender());
    if (!senderSocket) return;

    QByteArray data = senderSocket->readAll();
    qInfo() << "Получено сообщение:" << data;

    for (QTcpSocket *client : m_clientSockets)
    {
        if (client == senderSocket) continue;

        client->write(data);
    }
}

void QCore::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    qInfo() << "Клиент отключился:" << clientSocket->peerAddress().toString();
    m_clientSockets.removeAll(clientSocket);
    clientSocket->deleteLater();
}
