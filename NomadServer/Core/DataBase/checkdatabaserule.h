#ifndef CHECKDATABASERULE_H
#define CHECKDATABASERULE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class CheckDataBaseRule
{
public:
    CheckDataBaseRule();
    virtual ~CheckDataBaseRule() = default;

    virtual bool CheckTable(QSqlDatabase& db);
};

#endif // CHECKDATABASERULE_H
