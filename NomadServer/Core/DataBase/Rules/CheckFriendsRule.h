#ifndef CHECKFRIENDSRULE_H
#define CHECKFRIENDSRULE_H

#include "Core/DataBase/checkDBrule.h"

class CheckFriendsRule : public checkDBrule
{
public:
    bool CheckTable(QSqlDatabase& db) override
    {
        db.transaction();
        QSqlQuery query(db);

        QString createEnumSql =
            "DO $$ "
            "BEGIN "
            "    IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'friend_status_enum') THEN "
            "        CREATE TYPE friend_status_enum AS ENUM ('pending', 'accepted', 'blocked'); "
            "    END IF; "
            "END $$;";

        if (!query.exec(createEnumSql))
        {
            qCritical() << "Ошибка при создании ENUM friend_status_enum:" << query.lastError().text();
            db.rollback();
            return false;
        }

        QString createTableSql =
            "CREATE TABLE IF NOT EXISTS friends ("
            "    user_id UUID REFERENCES users(id) ON DELETE CASCADE,"
            "    friend_id UUID REFERENCES users(id) ON DELETE CASCADE,"
            "    status friend_status_enum NOT NULL DEFAULT 'pending',"
            "    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,"
            "    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,"
            "    PRIMARY KEY (user_id, friend_id),"
            "    CONSTRAINT check_self_friendship CHECK (user_id <> friend_id)"
            ");";

        if (!query.exec(createTableSql))
        {
            qCritical() << "Ошибка при создании таблицы friends:" << query.lastError().text();
            db.rollback();
            return false;
        }

        query.exec("DROP TRIGGER IF EXISTS update_friends_updated_at ON friends;");

        QString createTriggerSql = R"(
            CREATE TRIGGER update_friends_updated_at
                BEFORE UPDATE ON friends
                FOR EACH ROW
                EXECUTE FUNCTION update_updated_at_column();
        )";

        if (!query.exec(createTriggerSql))
        {
            qCritical() << "Ошибка при создании триггера для таблицы friends:" << query.lastError().text();
            db.rollback();
            return false;
        }

        if (db.commit())
        {
            qInfo() << "Таблица 'friends' успешно создана!";
            return true;
        } else
        {
            qCritical() << "Не удалось применить транзакцию для friends:" << db.lastError().text();
            db.rollback();
            return false;
        }
    }
};

#endif // CHECKFRIENDSRULE_H
