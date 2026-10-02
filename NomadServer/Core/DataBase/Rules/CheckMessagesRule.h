#ifndef CHECKMESSAGESRULE_H
#define CHECKMESSAGESRULE_H

#include "Core/DataBase/checkDBrule.h"

class CheckMessagesRule : public checkDBrule
{
public:
    bool CheckTable(QSqlDatabase& db) override
    {
        QSqlQuery query(db);

        QString createTableSql =
            "CREATE TABLE IF NOT EXISTS messages ("
            "    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),"
            "    channel_id UUID NOT NULL REFERENCES channels(id) ON DELETE CASCADE,"
            "    sender_id UUID NOT NULL,"
            "    content_type SMALLINT NOT NULL,"
            "    content TEXT NOT NULL,"
            "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,"
            "    updated_at TIMESTAMP NULL"
            ");";

        if (!query.exec(createTableSql))
        {
            qCritical() << "Ошибка при создании таблицы messages:" << query.lastError().text();
            return false;
        }

        QString alterMembersSql =
            "ALTER TABLE channel_members "
            "ADD CONSTRAINT fk_last_read_message "
            "FOREIGN KEY (last_read_message_id) REFERENCES messages(id) ON DELETE SET NULL;";

        QSqlQuery alterQuery(db);
        alterQuery.exec(alterMembersSql);

        return true;
    }
};

#endif // CHECKMESSAGESRULE_H
