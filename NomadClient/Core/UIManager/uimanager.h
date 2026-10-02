#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QObject>
#include "Core/UIManager/Server/servermanager.h"
#include "Core/UIManager/Friend/friendmanager.h"
#include "Core/UIManager/ChatManager/chatmanager.h"

class QUIManager  : public QObject
{
    Q_OBJECT


    Q_PROPERTY(int State READ getState WRITE setState NOTIFY StateChanged)
    int MainUIState = 0; // 0 - Personal | 1 - Server

public:

    QUIManager& operator=(const QUIManager&) = delete;
    QUIManager(const QUIManager&) = delete;
    static QUIManager& getInstance()
    {
        static QUIManager instance(nullptr);
        return instance;
    };

    int getState() { return MainUIState; }

    Q_INVOKABLE void setState(int NewNum)
    {
        if (MainUIState != NewNum)
        {
            MainUIState = NewNum;
            emit StateChanged();
        }
    }

    QServerManager* GetServerManager()
    {
        if(_ServerManager == nullptr)
        {
            _ServerManager = new QServerManager(nullptr);
        }
        return _ServerManager;
    }

    QFriendManager* GetFriendManager()
    {
        if(_FriendManager == nullptr)
        {
            _FriendManager = new QFriendManager(nullptr);
        }
        return _FriendManager;
    }

    QChatManager* GetChatManager()
    {
        if(_ChatManager == nullptr)
        {
            _ChatManager = new QChatManager(nullptr);
        }
        return _ChatManager;
    }


signals:
    void StateChanged();

private:
    QUIManager(QObject *parent = nullptr): QObject(parent) {};
    QServerManager* _ServerManager = nullptr;
    QFriendManager* _FriendManager = nullptr;
    QChatManager* _ChatManager = nullptr;
};

#endif // UIMANAGER_H
