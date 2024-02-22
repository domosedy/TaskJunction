#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <QMap>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "logging.hpp"
#include "query.hpp"

class ClientSocket;

class Database : public QObject {
    Q_OBJECT
signals:
    void executed_query(uint user_id, const QString &query);

public slots:

    void execute_query(uint user_id, std::shared_ptr<query> query) {
        query->execute();
        rDebug() << user_id << "\n";
        QString answer = "hello from database";

        emit executed_query(user_id, answer);
    }
};

class Server : public QObject {
    Q_OBJECT

    QTcpServer *server;
    quint16 port;
    Database *db;
    QMap<quintptr, ClientSocket *> id_of_all_connections;
    QMap<int, quintptr> pointer_to_authorized;

public:
    explicit Server(quint16);
    ~Server();

public slots:
    void newConnection();
    void removeConnection();
    void readyRead();
    void database_executed(uint user_id, const QString &response);
};

#endif