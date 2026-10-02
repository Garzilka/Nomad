#include "client.h"
#include <QDebug>

Client::Client(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket)
{
    m_socket->setParent(this);
    cryptolib = new QCryptoLib();

    //TODO: signal bad init -> destroy client
    if(!cryptolib->generateKeyPair()) return;

    SCyptsKeysData CryptoMessage(cryptolib->getPublicKeyDER().toBase64());
    sendMessage(CryptoMessage);

    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
}

Client::~Client()
{
    qInfo() << "Client: " << peerAddress() << " has been removed";
    delete cryptolib;
}

void Client::sendMessage(const SBaseMessageData &_Message)
{
    //TODO: Kill client
    if(m_socket == nullptr) return;

    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QJsonDocument doc(_Message.ToJSON());
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    if (cryptolib->hasSharedSecret())
    {
        data = cryptolib->encryptMessage(data);
    }

    // Префикс: 4 байта длины (big-endian)
    quint32 size = data.size();
    QByteArray packet;
    packet.append(reinterpret_cast<const char*>(&size), 4);
    packet.append(data);

    m_socket->write(packet);
    m_socket->flush();
}

QString Client::peerAddress() const
{
    return m_socket->peerAddress().toString();
}

void Client::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    while (m_buffer.size() >= 4)
    {
        quint32 size = *reinterpret_cast<const quint32*>(m_buffer.constData());
        if (size > 10 * 1024 * 1024)
        {
            qDebug() << "Слишком большой пакет, разрыв соединения";
            m_socket->disconnectFromHost();
            return;
        }
        if (m_buffer.size() < 4 + int(size)) break;  // ждём остаток

        QByteArray data = m_buffer.mid(4, size);
        m_buffer.remove(0, 4 + size);

        if (cryptolib->hasSharedSecret())
        {
            data = cryptolib->decryptMessage(data);
            if (data.isEmpty())
            {
                qDebug() << "Расшифровка не удалась";
                continue;
            }
        }

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << "Ошибка парсинга JSON:" << error.errorString();
            continue;
        }

        QJsonObject MainObject = doc.object();
        ETypeOfMessage TypeMessage = static_cast<ETypeOfMessage>(MainObject["TypeMessage"].toInt());

        if (TypeMessage == ETypeOfMessage::Crypto)
        {
            SCyptsKeysData CryptMessage(MainObject);
            QByteArray peerKey = QByteArray::fromBase64(CryptMessage.PublicKey.toUtf8());

            if (!cryptolib->computeSharedSecret(peerKey))
            {
                qDebug() << "Не удалось вычислить общий секрет";
                return;
            }

            qDebug() << "Успешный обмен ключами с" << peerAddress();
            return;
        }

        emit OnMessageReceived(this, MainObject, TypeMessage);
    }
}

void Client::onDisconnected()
{
    emit disconnected(this);
}
