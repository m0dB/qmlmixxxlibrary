#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QResource>

#include "database/mixxxdb.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.addImportPath(":/example.mixxx.org/imports");
    engine.load(QUrl("qrc:/example.mixxx.org/imports/Application/main.qml"));

    int result = app.exec();

    delete MixxxDb::singleton();

    return result;
}
