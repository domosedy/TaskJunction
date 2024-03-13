#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <QMap>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "logging.hpp"
#include "query.hpp"

class ClientSocket;

class Database {
    QMap<std::string, uint> all_users;
public:
    std::string execute_update_query(const update_query &query, uint user_id) {
        if (user_id == 0) {
            return "Unauthorized user";
        }
        return "It is update query from " + std::to_string(user_id) + " " + query.updated_type;
    }
    std::string execute_delete_query(const delete_query &query, uint user_id) {
        return "It is delete query from " + std::to_string(user_id) + " " + query.value_type;
    }
    std::string execute_create_query(const create_query &query, uint user_id) {
        return "It is create query from " + std::to_string(user_id) + " " + query.value_type;
    }

    std::pair<uint, std::string> execute_login_query(const login_query &query) {
        static uint id = 1;
        if (!all_users.contains(query.user_name)) {
            all_users[query.user_name] = id++;
        }

        return std::pair{all_users[query.user_name], "User tries to login " + query.user_name};
    }
};

class Server : public QObject {
    Q_OBJECT

    QTcpServer *server;
    quint16 port;
    Database *db;
    QMap<quintptr, ClientSocket *> id_of_all_connections;

public:
    explicit Server(quint16);
    ~Server();

public slots:
    void newConnection();
    void removeConnection();
    void readyRead();
    void execute_query(uint user_id, const query_type &query);

    // void database_executed(uint user_id, const QString &response);
};

#endif