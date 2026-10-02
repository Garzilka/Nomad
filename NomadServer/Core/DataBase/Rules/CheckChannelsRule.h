#ifndef CHECKCHANNELSRULE_H
#define CHECKCHANNELSRULE_H

#include "Core/DataBase/checkDBrule.h"

// Предполагаем, что класс checkDBrule подключен
// #include "checkdbrule.h"

class CheckChannelsRule : public checkDBrule
{
public:
    bool CheckTable(QSqlDatabase& db) override
    {
        QSqlQuery query(db);

        QString createTableSql =
            "CREATE TABLE IF NOT EXISTS channels ("
            "    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),"
            "    type SMALLINT NOT NULL,"
            "    server_id UUID NULL,"
            "    name VARCHAR(100) NULL,"
            "    parent_id UUID NULL,"
            "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL"
            ");";

        if (!query.exec(createTableSql)) {
            qCritical() << "Ошибка при создании таблицы channels:" << query.lastError().text();
            return false;
        }

        return true;
    }
};


#endif // CHECKCHANNELSRULE_H
