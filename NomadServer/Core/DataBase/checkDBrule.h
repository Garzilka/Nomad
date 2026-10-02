#ifndef CHECKDBRULE_H
#define CHECKDBRULE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class checkDBrule
{
public:
    checkDBrule();
    virtual ~checkDBrule() = default;

    virtual bool CheckTable(QSqlDatabase& db);
};

#endif // CHECKDBRULE_H
