#ifndef USERSCHECKRULE_H
#define USERSCHECKRULE_H

#include "Core/DataBase/checkdatabaserule.h"

class UsersCheckRule : public CheckDataBaseRule
{
public:
    UsersCheckRule();
    virtual ~ UsersCheckRule() = default;
    virtual bool CheckTable(QSqlDatabase& db) override;
};

#endif // USERSCHECKRULE_H
