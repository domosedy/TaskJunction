#include <QApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QQmlContext>
#include "listmodel.hpp"
#include "boardmodel.hpp"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection
    );

    qmlRegisterType<ListModel>("Client", 1, 0, "ClientListModel");
    qmlRegisterType<BoardModel>("Client", 1, 0, "ClientBoardModel");

    engine.load(url);
    return app.exec();
}
