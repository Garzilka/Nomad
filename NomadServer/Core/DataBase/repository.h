#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include "Core/Data/CoreData.h"
#include "Core/DataBase/checkDBrule.h"

class QRepository : public QObject
{

private:
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    checkDBrule* CurrentRule;

public:
    QRepository(QObject *parent = nullptr);
    bool Run();

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

    QPair<bool, QString> sendFriendRequest(const QString& senderGUID, const QString& targetUsername);
    bool handleFriendResponse(const QString& myGUID, const QString& targetGUID, int action);
    QList<SFriendInfo> getFriendsList(const QString& myGUID);
    QList<SFoundUserInfo> searchUsersByUsername(const QString& myGUID, const QString& searchQuery);

    QString findPrivateChannel(const QString& senderGUID, const QString& targetGUID);
    bool createPrivateChannel(const QString& roomGUID, const QString& senderGUID, const QString& targetGUID);
    QString getOrCreatePrivateChannel(const QString &senderGUID, const QString &targetGUID);
    bool saveMessage(const SMessageData& msg);

    QList<SMessageData> loadHistoryFromDB(const QString& roomGUID, int limit, int offset);
    bool isChannelExists(const QString& roomGUID);
    static QRepository& getInstance()
    {
        static QRepository instance(nullptr);
        return instance;
    };

    QString RegisterNewAccount(const SAuthorizationData& Data);
    QString CheckAuth(const SAuthorizationData& Data);
    bool HasAccount(const QString& _Username);
};

#endif // REPOSITORY_H
