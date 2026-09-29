#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include "Core/Data/CoreData.h"
#include "Core/DataBase/Rules/userscheckrule.h"

class QRepository : public QObject
{

private:
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    UsersCheckRule* CurrentRule;

public:
    QRepository(QObject *parent = nullptr);

    QRepository& operator=(const QRepository&) = delete;
    QRepository(const QRepository&) = delete;

    virtual ~QRepository()
    {
        if(CurrentRule)
        {
            delete CurrentRule;
        }
        db.close();
    };

    bool Run();
    static QRepository& getInstance()
    {
        static QRepository instance(nullptr);
        return instance;
    };

    bool RegisterNewAccount(const SAuthorizationData& Data);
    bool CheckAuth(const SAuthorizationData& Data);
    bool HasAccount(const QString& _Username);
};

#endif // REPOSITORY_H
