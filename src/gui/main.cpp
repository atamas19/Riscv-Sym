#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "cpuwrapper.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    CpuWrapper cpuWrapper;
    engine.rootContext()->setContextProperty("cpuWrapper", &cpuWrapper);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("RiscvSim", "Main");

    return app.exec();
}
