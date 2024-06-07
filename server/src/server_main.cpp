#include "jsonparser.hpp"
#include "server.hpp"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QStringList>

QtMessageHandler originalHandler = nullptr;

void logToFile(QtMsgType type, const QMessageLogContext &context,
               const QString &msg) {
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
              "{endif}] %{file}(%{line}): %{message}"));
  originalHandler = qInstallMessageHandler(logToFile);

  QCoreApplication app(argc, argv);

  QCommandLineParser parser;
  QCommandLineOption port_option("port");
  QCommandLineOption short_port_option("p");
  parser.addOption(port_option);
  parser.addOption(short_port_option);

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

  auto configParameters = parser::parse_config();

  if (!configParameters.contains("POSTGRES_USER") ||
      !configParameters.contains("POSTGRES_PASSWORD") ||
      !configParameters.contains("REMOTE_NAME")) {
    rDebug() << "Bad format of config";
    return 1;
  }

  Server server(PORT, configParameters);
  return app.exec();
}