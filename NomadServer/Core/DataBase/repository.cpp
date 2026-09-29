#include "repository.h"
#include "Core/Lib/cryptolib.h"

QRepository::QRepository(QObject *parent) : QObject(parent)
{

}

bool QRepository::Run()
{
    QSqlDatabase l_CheckDB = QSqlDatabase::addDatabase("QPSQL", "temp_init_connection");
    l_CheckDB.setHostName("127.0.0.1");
    l_CheckDB.setUserName("postgres");
    l_CheckDB.setPassword("12345678");
    l_CheckDB.setPort(5432);

    l_CheckDB.setDatabaseName("postgres");

    if (!l_CheckDB.open())
    {
        qCritical() << "REPOSITORY ERROR| Cannot connect to system 'postgres' DB:" << l_CheckDB.lastError().text();
        return false;
    }

    QSqlQuery query(l_CheckDB);
    query.prepare("SELECT 1 FROM pg_database WHERE datname = :dbname");
    query.bindValue(":dbname", "Nomad");

    if (!query.exec())
    {
        qCritical() << "REPOSITORY ERROR| Failed to check DB existence:" << query.lastError().text();
        return false;
    }

    if (!query.next())
    {
        qInfo() << "Database 'Nomad' does not exist. Creating...";

        if (!query.exec("CREATE DATABASE \"Nomad\""))
        {
            qCritical() << "REPOSITORY ERROR| Failed to create 'Nomad' DB:" << query.lastError().text();
            return false;
        }
        qInfo() << "Database 'Nomad' created successfully!";
    }
    else
    {
        qInfo() << "Database 'Nomad' already exists.";
    }

    l_CheckDB.close(); // Закрываем временное соединение

    QSqlDatabase::removeDatabase("temp_init_connection");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("Nomad");
    db.setUserName("postgres");
    db.setPassword("12345678");
    db.setPort(5432);

    if (!db.open())
{
        qCritical() << "REPOSITORY ERROR| Final PostgreSQL connect error to 'Nomad':" << db.lastError().text();
        return false;
    }
    CurrentRule = new UsersCheckRule();
    if(!CurrentRule->CheckTable(db))
    {
        return false;
    }
    delete CurrentRule;
    qDebug() << "REPOSITORY|PostgreSQL: Connection succesfully!";
    return true;
}

bool QRepository::RegisterNewAccount(const SAuthorizationData& Data)
{
    if(HasAccount(Data.Login)) return false;

    QByteArray SaltByte = QCryptoLib::generateSalt();
    QString Salt = QString::fromLatin1(SaltByte.toBase64());
    QString HashPass = QString::fromLatin1(QCryptoLib::hashPassword(Data.Password, SaltByte).toBase64());

    QString userEmail = Data.Email.isEmpty() ? QString("%1@example.com").arg(Data.Login) : Data.Email;

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO users "
                        "(username, display_name, email, password_hash, avatar_url, salt) VALUES "
                        "(:username, :display_name, :email, :password_hash, :avatar_url, :salt)");

    insertQuery.bindValue(":username", Data.Login);
    insertQuery.bindValue(":display_name", Data.Login);
    insertQuery.bindValue(":email", userEmail);
    insertQuery.bindValue(":password_hash", HashPass);
    insertQuery.bindValue(":salt", Salt);
    insertQuery.bindValue(":avatar_url", QVariant(QVariant::String));

    if (!insertQuery.exec())
    {
        qWarning() << "REPOSITORY ERROR | PostgreSQL INSERT error:" << insertQuery.lastError().text();
        return false;
    }

    qInfo() << "REPOSITORY | Пользователь успешно зарегистрирован:" << Data.Login;
    return true;
}

bool QRepository::HasAccount(const QString& _Username)
{
    QSqlQuery query;
    query.prepare("SELECT EXISTS(SELECT 1 FROM users WHERE username = :username)");
    query.bindValue(":username", _Username);

    if (!query.exec())
    {
        qWarning() << "REPOSITORY ERROR|PostgreSQL check login error: " << query.lastError().text();
        return true;
    }

    if (query.next())
    {
        return query.value(0).toBool();
    }
    return true;
}

bool QRepository::CheckAuth(const SAuthorizationData& Data)
{
    QSqlQuery query;

    query.prepare("SELECT password_hash, salt FROM users WHERE username = :username");
    query.bindValue(":username", Data.Login);

    if (!query.exec())
    {
        qWarning() << "REPOSITORY ERROR | PostgreSQL SELECT login error:" << query.lastError().text();
        return false;
    }


    if (!query.next())
    {
        qWarning() << "REPOSITORY | Попытка входа: пользователь" << Data.Login << "не найден.";
        return false;
    }


    QString dbHash = query.value("password_hash").toString();
    QString dbSaltBase64 = query.value("salt").toString();
    QByteArray saltByte = QByteArray::fromBase64(dbSaltBase64.toLatin1());


    QString computedHash = QString::fromLatin1(
        QCryptoLib::hashPassword(Data.Password, saltByte).toBase64()
        );


    if (computedHash != dbHash)
    {
        qWarning() << "REPOSITORY | Попытка входа: неверный пароль для пользователя" << Data.Login;
        return false;
    }

    qInfo() << "REPOSITORY | Пользователь успешно авторизован:" << Data.Login;

    return true;
}
