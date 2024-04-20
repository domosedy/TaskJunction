#include "server.hpp"
#include <functional>
#include <memory>
#include "client_socket.hpp"
#include "element_classes.hpp"
#include "logging.hpp"
#include "query.hpp"
// #include "datas.hpp"

enum class RequestType {
    ERROR,
    CREATE,
    DELETE,
    UPDATE,
    LOGIN,
    GET_BOARDS_INFO
};

// How to emit when my connection is not authorized?
Server::Server(quint16 port)
    : port(port), db("postgres", "postgres", "localhost", "") {
    server = new QWebSocketServer(QStringLiteral("Task Server"),
            QWebSocketServer::NonSecureMode, this);
    // db.fill_query_name_to_sql_command();

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if (!server->listen(QHostAddress::Any, port)) {
        rDebug() << QObject::tr("Unable to start server: %1")
                        .arg(server->errorString());
        std::exit(1);
    }
}

Server::~Server() {
    server->close();
    qDeleteAll(unauthorized_connections.begin(), unauthorized_connections.end());
    for (auto &connection : authorized_connections) {
        delete connection;
    }
}

void Server::newConnection() {
    QWebSocket *clientSocket = server->nextPendingConnection();
    ClientSocket *clientSock = new ClientSocket(clientSocket, 0);
    
    connect(
        clientSock,
        SIGNAL(request_to_database(uint, query_type)), this,
        SLOT(execute_query(uint, query_type))
    );

    connect(
        clientSock, SIGNAL(disconnected()), this,
        SLOT(removeConnection())
    );

    unauthorized_connections << clientSock;
}

void Server::execute_query(uint user_id, const query_type &query) {
    auto clientSocket = dynamic_cast<ClientSocket *>(sender());
    auto client_id = clientSocket->get_client_id();

    rDebug() << client_id;

    auto result_code = std::visit(
        [](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, update_query>) {
                return RequestType::UPDATE;
            } else if constexpr (std::is_same_v<T, create_query>) {
                return RequestType::CREATE;
            } else if constexpr (std::is_same_v<T, delete_query>) {
                return RequestType::DELETE;
            } else if constexpr (std::is_same_v<T, login_query>) {
                return RequestType::LOGIN;
            } else if constexpr (std::is_same_v<T, get_boards_info_query>) {
                return RequestType::GET_BOARDS_INFO;
            }
            return RequestType::ERROR;
        },
        query
    );

    std::string result = "";
    std::pair<QString, quint32> res;
    std::size_t board_id = 0;

    switch (result_code) {
        case RequestType::UPDATE:
            result = execute_update_query(std::get<update_query>(query)).toStdString();
            board_id = std::get<update_query>(query).board_id;
            break;
        case RequestType::CREATE:
            res = execute_create_query(std::get<create_query>(query), client_id);
            result = res.first.toStdString();
            board_id = res.second;
            break;
        case RequestType::DELETE:
            result = execute_delete_query(std::get<delete_query>(query))
                         .toStdString();
            board_id = std::get<delete_query>(query).board_id;
            break;
        case RequestType::LOGIN:
            res = execute_login_query(std::get<login_query>(query));
            if (res.second != 0) {
                clientSocket->set_client_id(res.second);
                unauthorized_connections.removeAll(clientSocket);
                authorized_connections[res.second] = clientSocket;
            }
            result = res.first.toStdString();
            break;
        case RequestType::GET_BOARDS_INFO:
            result = execute_get_query(std::get<get_boards_info_query>(query))
                         .toStdString();
            break;
        default:
            result = std::move(error{"An error occured"}.to_json());
    }

    // auto id = clientSocket->get_client_id();
    clientSocket->sendData(result.c_str());

    // auto group_id = 
    
    // auto send_result = [&result] ()
}

QString Server::execute_update_query(const update_query &query) {
    auto result = db.update_command(
        (query.updated_type + "_signature").c_str(), query.value_name.c_str(),
        query.new_value.c_str(), query.value_id
    );

    return result ? error{"Ok"}.to_json().c_str()
                  : error{"An error occured"}.to_json().c_str();
}

QString Server::execute_delete_query(const delete_query &query) {
    // if (!db.check_user_rights(user_id, query.))
    auto result = db.delete_command(
        (query.value_type + "_signature").c_str(), query.value_id
    );

    return result ? error{"Ok"}.to_json().c_str()
                  : error{"An error occured"}.to_json().c_str();
}

std::pair<QString, quint32>
Server::execute_create_query(const create_query &query, quint32 user_id) {
    if (user_id == 0) {
        return {error{"You are not authorized"}.to_json().c_str(), 0};
    }
    
    quint32 result = 0;

    rDebug() << user_id;
    rDebug() << query.value_type.c_str();

    std::string jsoned_object = "";
    quint32 board_id = query.board_id;

    if (query.value_type == "board") {
        rDebug() << "Hello from board";
        rDebug() << query.value_name.c_str();
        rDebug() << query.value_description.c_str();

        // FIXME: replace 1 to real group_id
        result = db.insert_board(
            1, query.value_name.c_str(), query.value_description.c_str()
        );

        board_id = result;
        if (result != 0) {
            jsoned_object = db.select_board(result).to_json();
        }
    } else if (query.value_type == "list") {
        if (!db.check_user_rights(user_id, query.board_id)) {
            return {error{"Ypu don't have rights on this group"}.to_json().c_str(), 0};
        }
        result = db.insert_list(
            query.parent_id, query.value_name.c_str(),
            query.value_description.c_str()
        );

        if (result != 0) {
            jsoned_object = db.select_list(result).to_json();
        }
    } else if (query.value_type == "card") {
        if (!db.check_user_rights(user_id, query.board_id)) {
            return {error{"Ypu don't have rights on this group"}.to_json().c_str(), 0};
        }

        result = db.insert_card(
            query.parent_id, query.value_name.c_str(),
            query.value_description.c_str()
        );

        if (result != 0) {
            jsoned_object = db.select_card(result).to_json();
        }
    }

    return result > 0
               ? std::pair<QString, quint32>{create_response{result, query.value_type.c_str(),
                                 query.board_id, query.list_id,
                                 query.card_id,  jsoned_object}
                     .to_json()
                     .c_str(), board_id}
               : std::pair<QString, quint32>{error{"An error occured"}.to_json().c_str(), 0};
}

std::pair<QString, quint32> Server::execute_login_query(const login_query &query
) {
    auto id =
        db.authorize_user(query.user_name.c_str(), query.password.c_str());
    login response{id != 0};

    rDebug() << "user id is: " << id;
    if (response.m_response) {
        auto all_groups = db.get_user_groups(id);
        for (auto &group_iterator : all_groups) {
            response.m_boards += db.get_group_boards(group_iterator.m_group_id);
        }

        rDebug() << response.m_boards.size();
    }

    return {response.to_json().c_str(), id};
}

QString Server::execute_get_query(const get_boards_info_query &query) {
    rDebug() << "Get query";
    board result = db.select_board(query.id);
    rDebug() << query.id;
    result.m_lists = db.get_board_lists(query.id);
    for (auto &list : result.m_lists) {
        list.m_cards = db.get_list_cards(list.m_list_id);

        for (auto &card : list.m_cards) {
            card.m_tags = db.get_card_tags(card.m_card_id);
        }
    }

    return result.to_json().c_str();
}

void Server::removeConnection() {
    auto clientSocket = dynamic_cast<ClientSocket *>(sender());
    if (!clientSocket) {
        rDebug() << "Error in casting in signal";
        return;
    }

    unauthorized_connections.removeAll(clientSocket);
    authorized_connections.remove(clientSocket->get_client_id());
    delete clientSocket;
}