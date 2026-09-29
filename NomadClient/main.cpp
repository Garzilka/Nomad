#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Core/connection.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QConnection& Connection = QConnection::getInstance();
    engine.rootContext()->setContextProperty("ClientConnection", &Connection);



    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("NomadClient", "Main");


    return QGuiApplication::exec();
}
