#include <QApplication>
#include <QQmlContext>
#include <QtQml/QQmlApplicationEngine>
#include "boardmodel.hpp"
#include "client.hpp"
#include "listmodel.hpp"

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
    qmlRegisterType<Client>("Client", 1, 0, "Client");

    Client *client = new Client();
    engine.rootContext()->setContextProperty("mainClient", client);
    engine.load(url);
    return app.exec();
}
