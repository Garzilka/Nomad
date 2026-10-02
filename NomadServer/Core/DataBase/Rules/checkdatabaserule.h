#ifndef CHECKDATABASERULE_H
#define CHECKDATABASERULE_H

#include "Core/DataBase/checkDBrule.h"

class CheckDataBaseRule : public checkDBrule
{
public:
    CheckDataBaseRule() {};
    virtual bool CheckTable(QSqlDatabase& db) override
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

        l_CheckDB.close();

        QSqlDatabase::removeDatabase("temp_init_connection");
        return true;
    }
};

#endif // CHECKDATABASERULE_H
