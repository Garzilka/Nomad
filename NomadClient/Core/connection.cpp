#include "connection.h"

QConnection::QConnection(QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket(this);
    cryptolib = new QCryptoLib();

    connect(m_socket, &QTcpSocket::connected, this, &QConnection::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &QConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &QConnection::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &QConnection::onErrorOccurred);
}

void QConnection::connectToServer(const QString &host, quint16 port)
{
    qDebug() << "Connect to" << host << ":" << port;
    if (m_socket->state() == QAbstractSocket::ConnectedState) return;

    m_socket->connectToHost(host, port);
}

bool QConnection::sendMessage(const SBaseMessageData &_Message)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        QJsonDocument doc(_Message.ToJSON());
        QByteArray data = doc.toJson(QJsonDocument::Compact);

        if (cryptolib->hasSharedSecret())
        {
            data = cryptolib->encryptMessage(data);
        }

        quint32 size = data.size();
        QByteArray packet;
        packet.append(reinterpret_cast<const char*>(&size), 4);
        packet.append(data);

        m_socket->write(packet);
        m_socket->flush();
        return true;
    }
    else
    {
        qWarning() << "ERROR: Server not responding. Message was not sent.";
        return false;
    }
}

void QConnection::authorization(const QString &Login, const QString &Password)
{
    SAuthorizationData sendData(ETypeOfMessage::Login, Login, "", Password);
    sendMessage(sendData);
}

void QConnection::registration(const QString &Login, const QString &Email, const QString &Password)
{
    SAuthorizationData sendData(ETypeOfMessage::Registration, Login, Email, Password);
    sendMessage(sendData);
};

void QConnection::onConnected()
{
    qDebug() << "Successful connection!";
}

void QConnection::onReadyRead()
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

        if (m_buffer.size() < 4 + int(size)) break;

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
        SBaseMessageData Data(MainObject);

        if (Data.Response && Data.TypeMessage == ETypeOfMessage::Crypto)
        {
            SCyptsKeysData CryptMessage(MainObject);
            QByteArray peerKey = QByteArray::fromBase64(CryptMessage.PublicKey.toUtf8());

            if (!cryptolib->generateKeyPair())
            {
                qDebug() << "Не удалось сгенерировать пару ключей";
                continue;
            }

            SCyptsKeysData ResponseCryptMessage(cryptolib->getPublicKeyDER().toBase64());
            sendMessage(ResponseCryptMessage);

            if (!cryptolib->computeSharedSecret(peerKey))
            {
                qDebug() << "Не удалось вычислить общий секрет";
                continue;
            }

            qDebug() << "Шифрование установлено";
            setSecuritySuccessfully(true);
            emit OnKeyExchangeComplete();
            continue;
        }

        if (Data.Response && Data.TypeMessage == ETypeOfMessage::AuthResponse)
        {
            setState(1);
            emit OnAuthComplete();
        }
    }
}

void QConnection::onDisconnected()
{
    qDebug() << "Disconnect!";
    m_buffer.clear();
}

void QConnection::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qCritical() << "ERROR: Socket:" << m_socket->errorString();
}
