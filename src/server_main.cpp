#include <QCoreApplication>
#include "server.hpp"

int main(int argc, char *argv[]) {
#ifndef PORT
    #define PORT 3030
#endif
    QCoreApplication a(argc, argv);
    Server server(PORT);
    return a.exec();
}