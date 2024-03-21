#include "server.hpp"
#include <functional>
#include <memory>
#include "client_socket.hpp"
#include "logging.hpp"
#include "query.hpp"
#include "element_classes.hpp"
// #include "datas.hpp"

enum class RequestType { ERROR, CREATE, DELETE, UPDATE, LOGIN, GET_BOARDS_INFO };

// How to emit when my connection is not authorized?
Server::Server(quint16 port) : port(port), db("postgres", "postgres", "localhost", "") {
    server = new QTcpServer(this);
    db.fill_query_name_to_sql_command();

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if (!server->listen(QHostAddress::Any, port)) {
        rDebug() << QObject::tr("Unable to start server: %1")
                        .arg(server->errorString());
        std::exit(1);
    }

}

Server::~Server() {
}

void Server::newConnection() {
    QTcpSocket *clientSocket = server->nextPendingConnection();
    ClientSocket *clientSock = new ClientSocket(clientSocket, 0);
    auto socket_id = clientSock->get_socket_id();
    id_of_all_connections[socket_id] = clientSock;

    connect(
        id_of_all_connections[socket_id],
        SIGNAL(request_to_database(uint, query_type)), this,
        SLOT(execute_query(uint, query_type))
    );

    connect(
        id_of_all_connections[socket_id], SIGNAL(disconnected()), this,
        SLOT(removeConnection())
    );
}

void Server::readyRead() {
    auto clientSocket = dynamic_cast<QTcpSocket *>(sender());
    if (!clientSocket) {
        rDebug() << "Error in casting in signal";
        return;
    }

    auto address = clientSocket->peerAddress();
    QByteArray data = clientSocket->readAll();
    rDebug() << "Recevied from " << address.toString() << ": " << data;

    clientSocket->write(data);
}

void Server::execute_query(uint user_id, const query_type &query) {
    auto clientSocket = dynamic_cast<ClientSocket *>(sender());
    auto client_id = clientSocket->get_client_id();
    auto socket_id = clientSocket->get_socket_id();

    rDebug() << client_id;

    auto result_code = std::visit([](auto &&arg) {
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
    }, query);

    std::string result = "";
    std::pair<QString, quint32> res;
    switch (result_code) {
        case RequestType::UPDATE:
            result = execute_update_query(std::get<update_query>(query)).toStdString();
            break;
        case RequestType::CREATE:
            result = execute_create_query(std::get<create_query>(query), client_id).toStdString();
            break;
        case RequestType::DELETE:
            result = execute_delete_query(std::get<delete_query>(query)).toStdString();
            break;
        case RequestType::LOGIN:
            res = execute_login_query(std::get<login_query>(query));
            id_of_all_connections[socket_id]->set_client_id(res.second);
            result = res.first.toStdString();
            break;
        case RequestType::GET_BOARDS_INFO:
            result = execute_get_query(client_id).toStdString();
        default:
            result = std::move(error{"An error occured"}.to_json());        
    }

    clientSocket->sendData(result.c_str());    
}

QString Server::execute_update_query(const update_query &query) {
    auto result = db.update_command(
                    (query.updated_type + "_signature").c_str(),
                    query.value_name.c_str(),
                    query.new_value.c_str(),
                    query.value_id);

    return result ? error{"Ok"}.to_json().c_str() :
                    error{"An error occured"}.to_json().c_str();
}

QString Server::execute_delete_query(const delete_query &query) {
    auto result = db.delete_command(
                    (query.value_type + "_signature").c_str(),
                    query.value_id);

    return result ? error{"Ok"}.to_json().c_str() :
                    error{"An error occured"}.to_json().c_str();
}

QString Server::execute_create_query(const create_query &query, quint32 user_id) {
    if (user_id == 0) {
        return error{"You are not authorized"}.to_json().c_str();
    }

    bool result = true;
    rDebug() << user_id;
    rDebug() << query.value_type;
    if (query.value_type == "board") {
        rDebug() << "Hello from board";
        rDebug() << query.value_name;
        rDebug() << query.value_description;

        result = db.insert_board(
            user_id, 
            query.value_name.c_str(), 
            query.value_description.c_str());
    } else if (query.value_type == "list") {
        result = db.insert_list(
            query.parent_id, 
            query.value_name.c_str(), 
            query.value_description.c_str());
    } else if (query.value_type == "card") {
        result = db.insert_card(
            query.parent_id, 
            query.value_name.c_str(), 
            query.value_description.c_str());
    }

    return result ? error{"Ok"}.to_json().c_str() :
                    error{"An error occured"}.to_json().c_str();
}

std::pair<QString, quint32> Server::execute_login_query(const login_query &query) {
    auto id = db.authorize_user(query.user_name.c_str(), query.password.c_str());
    login response{id != 0};

    rDebug() << "user id is: " << id;
    if (response.m_response) {
        response.m_boards = db.get_user_boards(id);
        rDebug() << response.m_boards.size();
    }

    return {response.to_json().c_str(), id};
}

QString Server::execute_get_query(quint32 client_id) {
    if (client_id == 0) {
        return error{"You are not authorized"}.to_json().c_str();
    }

    board result = db.select_board(client_id);
    
    result.m_lists = db.get_board_lists(client_id);
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

    id_of_all_connections.remove(clientSocket->get_socket_id());
    delete clientSocket;
}