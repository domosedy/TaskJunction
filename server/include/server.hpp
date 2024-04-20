#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <QMap>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "database.hpp"
#include <QWebSocketServer>
#include "logging.hpp"
#include "query.hpp"

using namespace database;

class ClientSocket;

class Server : public QObject {
    Q_OBJECT

    QWebSocketServer *server;
    quint16 port;
    db_manager db;
    QMap<quint32, ClientSocket *> authorized_connections;
    QList<ClientSocket*> unauthorized_connections;

    QString execute_update_query(const update_query &query);
    std::pair<QString, quint32> execute_create_query(const create_query &query, quint32 id);
    QString execute_delete_query(const delete_query &query);
    QString execute_get_query(const get_boards_info_query &query);

    std::pair<QString, quint32> execute_login_query(const login_query &query);

public:
    explicit Server(quint16);
    ~Server();

public slots:
    void newConnection();
    void removeConnection();
    void execute_query(uint user_id, const query_type &query);

    // void database_executed(uint user_id, const QString &response);
};

#endif