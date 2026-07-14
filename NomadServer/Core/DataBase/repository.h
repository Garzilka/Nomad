#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include "Core/Data/CoreData.h"

class QRepository : public QObject
{

private:
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

public:
    QRepository(QObject *parent = nullptr);

    QRepository& operator=(const QRepository&) = delete;
    QRepository(const QRepository&) = delete;

    static QRepository& getInstance()
    {
        static QRepository instance(nullptr);
        return instance;
    };

    bool RegisterNewAccount(SAuthorizationData Data);
    bool HasAccount(QString Login);
    bool CheckAuth(SAuthorizationData Data);
};

#endif // REPOSITORY_H
