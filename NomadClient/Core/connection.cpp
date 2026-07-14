#include "connection.h"

QConnection::QConnection(QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected, this, &QConnection::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &QConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &QConnection::onDisconnected);

    connect(m_socket, &QTcpSocket::errorOccurred, this, &QConnection::onErrorOccurred);
}

void QConnection::connectToServer(const QString &host, quint16 port)
{
    qDebug() << "Conect to " << host << ":" << port;
    if (m_socket->state() == QAbstractSocket::ConnectedState) return;

    m_socket->connectToHost(host, port);
}

bool QConnection::sendMessage(SBaseMessageData* _Message)
{
    if(_Message == nullptr) return false;

    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        QJsonDocument doc(_Message->ToJSON());
        QString jsonString = doc.toJson(QJsonDocument::Indented);

        m_socket->write(jsonString.toUtf8());
        m_socket->flush();
        qDebug() << "The message has been sent: " << jsonString;
        return true;
    }
    else
    {
        qWarning() << "ERROR: Server not responding. Message was not sent.";
    }
    return false;
}

void QConnection::onConnected()
{
    qDebug() << "Successful connection!";
}

void QConnection::onReadyRead()
{
    QByteArray data = m_socket->readAll();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Ошибка парсинга JSON:" << error.errorString();
        return;
    }

    QJsonObject MainObject = doc.object();
    SBaseMessageData Data(MainObject);

    if(Data.TypeMessage == ETypeOfMessage::AuthResponse)
    {
        if(Data.Response)
        {
            emit OnAuthComplete();
        }
    }
}

void QConnection::onDisconnected()
{
    qDebug() << "Disconnect!";
}

void QConnection::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qCritical() << "ERROR: Socket:" << m_socket->errorString();
}
