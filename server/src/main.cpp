#include <QDebug>
#include "logging.hpp"

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

int main() {
    qSetMessagePattern(
        QString("[%{time yyyy:MM:dd hh:mm:ss t} "
                "%{if-debug}Debug%{endif}%{if-info}Info%{endif}%{if-warning}"
                "Warning%{endif}%{if-critical}Critical%{endif}%{if-fatal}Fatal%"
                "{endif}] %{file}(%{line}): %{message}")
    );
    originalHandler = qInstallMessageHandler(logToFile);

    rDebug() << "Hello world";
}
