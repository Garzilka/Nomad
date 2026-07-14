#include "repository.h"

QRepository::QRepository(QObject *parent)
{
    db.setHostName("127.0.0.1");
    db.setDatabaseName("Nomad");
    db.setUserName("postgres");
    db.setPassword("12345678");
    db.setPort(5432);

    if (!db.open())
    {
        qCritical() << "REPOSITORY ERROR|PostgreSQL connect error: " << db.lastError().text();
        return;
    }

    qDebug() << "REPOSITORY|PostgreSQL: Connection succesfully!";
}

bool QRepository::RegisterNewAccount(SAuthorizationData Data)
{
    if(HasAccount(Data.Login)) return false;

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO users (login, pass) VALUES (:login, :pass)");
    insertQuery.bindValue(":login", Data.Login);
    insertQuery.bindValue(":pass", Data.Password);

    if (!insertQuery.exec())
    {
        qWarning() << "REPOSITORY ERROR|PostgreSQL INSERT error: " << insertQuery.lastError().text();
        return false;
    }
    return true;
}

bool QRepository::HasAccount(QString Login)
{
    QSqlQuery query;
    query.prepare("SELECT EXISTS(SELECT 1 FROM users WHERE login = :login)");
    query.bindValue(":login", Login);

    if (!query.exec())
    {
        qWarning() << "REPOSITORY ERROR|PostgreSQL check login error: " << query.lastError().text();
        return true; // Или генерируйте исключение в зависимости от архитектуры
    }

    if (query.next())
    {
        return query.value(0).toBool();
    }
    return true;
}

bool QRepository::CheckAuth(SAuthorizationData Data)
{
    QSqlQuery query;
    query.prepare("SELECT EXISTS(SELECT 1 FROM users WHERE login = :login AND pass = :pass)");
    query.bindValue(":login", Data.Login);
    query.bindValue(":pass", Data.Password);

    if (!query.exec())
    {
        qWarning() << "REPOSITORY ERROR|PostgreSQL check login error: " << query.lastError().text();
        return false;
    }

    if (query.next())
    {
        return query.value(0).toBool();
    }
    return false;
}
