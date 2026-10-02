#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H
#pragma once

#include <QObject>

class QServerManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int State READ getState WRITE setState NOTIFY StateChanged)

    int ServerState = 0; // 0 - Voice/Text Chat | 1 - KanBan | 2 - Docs

public:
    QServerManager(QObject *parent = nullptr) : QObject(parent) {};

    Q_INVOKABLE void setState(int NewNum)
    {
        if (ServerState != NewNum)
        {
            ServerState = NewNum;
            emit StateChanged();
        }
    }

    int getState()
    {
        return ServerState;
    }

signals:
    void StateChanged();
};

#endif // SERVERMANAGER_H
