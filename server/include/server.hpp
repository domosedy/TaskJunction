#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <QMap>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "logging.hpp"
#include "query.hpp"
#include "database.hpp"

using namespace database;

class ClientSocket;

class Server : public QObject {
    Q_OBJECT

    QTcpServer *server;
    quint16 port;
    db_manager db;
    QMap<quintptr, ClientSocket *> id_of_all_connections;

    QString execute_update_query(const update_query &query);
    QString execute_create_query(const create_query &query, quint32 id);
    QString execute_delete_query(const delete_query &query);
    QString execute_get_query(const get_boards_info_query &query);

    std::pair<QString, quint32> execute_login_query(const login_query &query);

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