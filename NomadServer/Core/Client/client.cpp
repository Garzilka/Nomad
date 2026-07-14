#include "client.h"
#include <QDebug>

Client::Client(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket)
{
    m_socket->setParent(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
}

Client::~Client()
{
    qInfo() << "Client: " << peerAddress() << " has been removed";
}

void Client::sendMessage(SBaseMessageData &data)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QJsonDocument doc(data.ToJSON());
    QString jsonString = doc.toJson(QJsonDocument::Indented);

    m_socket->write(jsonString.toUtf8());
    m_socket->flush();
}

QString Client::peerAddress() const
{
    return m_socket->peerAddress().toString();
}

void Client::onReadyRead()
{
    QByteArray data = m_socket->readAll();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Ошибка парсинга JSON:" << error.errorString();
        return;
    }

    QJsonObject MainObject = doc.object();
    emit OnMessageReceived(this, MainObject);
}

void Client::onDisconnected()
{
    emit disconnected(this);
}
