#include <QCommandLineParser>
#include <QCoreApplication>
#include <QStringList>
#include "server.hpp"

QtMessageHandler originalHandler = nullptr;

void logToFile(
    QtMsgType type,
    const QMessageLogContext &context,
    const QString &msg
) {
    QString message = qFormatLogMessage(type, context, msg);
    static FILE *f = fopen("logs.log", "a");
    fprintf(f, "%s\n", qPrintable(message));
    fflush(f);

    if (originalHandler) {
        originalHandler(type, context, msg);
    }
}

int main(int argc, char *argv[]) {
    // TODO: replace with QCommandLineParser

    int PORT = 3030;

    qSetMessagePattern(
        QString("[%{time yyyy:MM:dd hh:mm:ss t} "
                "%{if-debug}Debug%{endif}%{if-info}Info%{endif}%{if-warning}"
                "Warning%{endif}%{if-critical}Critical%{endif}%{if-fatal}Fatal%"
                "{endif}] %{file}(%{line}): %{message}")
    );
    originalHandler = qInstallMessageHandler(logToFile);

    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    QCommandLineOption port_option("port");
    QCommandLineOption short_port_option("p");
    parser.addOption(port_option);
    parser.addOption(short_port_option);

    // parser.addPositionalArgument("port", QCoreApplication::translate("main",
    // "Set port to main.")); parser.addPositionalArgument("p",
    // QCoreApplication::translate("main", "Set port to main."));

    parser.process(app);

    auto args = parser.positionalArguments();

    try {
        if (!args.empty()) {
            PORT = args.first().toInt();
        }
    } catch (...) {
        rDebug() << "Port error option";
        return 0;
    }

    Server server(PORT);
    return app.exec();
}