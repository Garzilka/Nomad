#include "userscheckrule.h"

UsersCheckRule::UsersCheckRule() {}

bool UsersCheckRule::CheckTable(QSqlDatabase& db)
{
    if(db.tables().contains("Users", Qt::CaseInsensitive))
    {
        return true;
    }

    db.transaction();


    QSqlQuery query;

    if (!query.exec("CREATE EXTENSION IF NOT EXISTS \"uuid-ossp\";"))
    {
        qWarning() << "Не удалось создать расширение uuid-ossp:" << query.lastError().text();
        db.rollback();
        return false;
    }

    QString createEnumSql =
        "DO $$ "
        "BEGIN "
        "    IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'user_status_enum') THEN "
        "        CREATE TYPE user_status_enum AS ENUM ('online', 'idle', 'dnd', 'offline'); "
        "    END IF; "
        "END $$;";

    if (!query.exec(createEnumSql))
    {
        qWarning() << "Не удалось создать ENUM тип статусов:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // Шаг 3: Создаем саму таблицу users
    QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
            username VARCHAR(32) NOT NULL UNIQUE CHECK (username ~ '^[a-zA-Z0-9_]+$'),
            display_name VARCHAR(50) NOT NULL,
            email VARCHAR(255) NOT NULL UNIQUE CHECK (email ~* '^[A-Za-z0-9._%-]+@[A-Za-z0-9.-]+[A-Za-z]{2,4}$'),
            password_hash VARCHAR(255) NOT NULL,
            avatar_url VARCHAR(2048) DEFAULT NULL,
            status user_status_enum NOT NULL DEFAULT 'offline',
            created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
            updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL
        );
    )";

    if (!query.exec(createTableSql))
    {
        qWarning() << "Не удалось создать таблицу users:" << query.lastError().text();
        db.rollback();
        return false;
    }

    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_users_username ON users (username);"))
    {
        qWarning() << "Не удалось создать индекс для username:" << query.lastError().text();
        db.rollback();
        return false;
    }

    QString createFunctionSql = R"(
        CREATE OR REPLACE FUNCTION update_updated_at_column()
        RETURNS TRIGGER AS $$
        BEGIN
            NEW.updated_at = CURRENT_TIMESTAMP;
            RETURN NEW;
        END;
        $$ language 'plpgsql';
    )";

    if (!query.exec(createFunctionSql))
    {
        qWarning() << "Не удалось создать триггерную функцию:" << query.lastError().text();
        db.rollback();
        return false;
    }

    query.exec("DROP TRIGGER IF EXISTS update_users_updated_at ON users;");

    QString createTriggerSql = R"(
        CREATE TRIGGER update_users_updated_at
            BEFORE UPDATE ON users
            FOR EACH ROW
            EXECUTE FUNCTION update_updated_at_column();
    )";

    if (!query.exec(createTriggerSql))
    {
        qWarning() << "Не удалось привязать триггер к таблице:" << query.lastError().text();
        db.rollback();
        return false;
    }

    if (db.commit())
    {
        qInfo() << "Таблица 'users' и вся сопутствующая структура успешно созданы!";
        return true;
    } else {
        qWarning() << "Не удалось применить транзакцию (commit failed):" << db.lastError().text();
        db.rollback();
        return false;
    }
}
