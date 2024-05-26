#include <QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <iostream>
#include <string>
#include "database.hpp"

int main(int argc, char *argv[]) {
    if (argc < 5) {
        qDebug() << "wrong arguments format";
        return 1;
    }
    using namespace database;
    db_manager db_manager(argv[1], argv[2], argv[3], argv[4]);

    return 0;
}
