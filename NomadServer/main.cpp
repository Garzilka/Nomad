#include <QCoreApplication>
#include "Core/core.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCore* MyCoreServer = new QCore(nullptr);

    if(!MyCoreServer->startServer(55))
    {
        delete MyCoreServer;
        return -1;
    }

    return a.exec();
}
