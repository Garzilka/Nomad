#ifndef CHECKCHANNELMEMBERSRULE_H
#define CHECKCHANNELMEMBERSRULE_H

#include "Core/DataBase/checkDBrule.h"

class CheckChannelMembersRule : public checkDBrule
{
public:
    bool CheckTable(QSqlDatabase& db) override
    {
        QSqlQuery query(db);

        QString createTableSql =
            "CREATE TABLE IF NOT EXISTS channel_members ("
            "    channel_id UUID REFERENCES channels(id) ON DELETE CASCADE,"
            "    user_id UUID NOT NULL,"
            "    joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,"
            "    last_read_message_id UUID NULL,"
            "    PRIMARY KEY (channel_id, user_id)"
            ");";

        if (!query.exec(createTableSql)) {
            qCritical() << "Ошибка при создании таблицы channel_members:" << query.lastError().text();
            return false;
        }

        return true;
    }
};


#endif // CHECKCHANNELMEMBERSRULE_H
