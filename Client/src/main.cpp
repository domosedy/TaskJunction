#include <QApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QQmlContext>
#include "listmodel.hpp"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    //Client client;

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

    //engine.rootContext()->setContextProperty("ListModel", new ListModel());
    engine.load(url);
    return app.exec();
}
