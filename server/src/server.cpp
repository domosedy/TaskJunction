#include "server.hpp"
#include <functional>
#include <memory>
#include "client_socket.hpp"
#include "logging.hpp"
#include "query.hpp"
#include "datas.hpp"

enum class RequestType { ERROR, CREATE, DELETE, UPDATE, LOGIN, GET_BOARDS_INFO };

// How to emit when my connection is not authorized?
Server::Server(quint16 port) : port(port) {
    server = new QTcpServer(this);
    db = new Database();

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
    std::pair<uint, std::string> returned_value;
    switch (result_code) {
        case RequestType::UPDATE:
            result = db->execute_update_query(std::get<update_query>(query), client_id);
            break;
        case RequestType::CREATE:
            result = db->execute_create_query(std::get<create_query>(query), client_id);
            break;
        case RequestType::DELETE:
            result = db->execute_delete_query(std::get<delete_query>(query), client_id);
            break;
        case RequestType::LOGIN:
            returned_value = db->execute_login_query(std::get<login_query>(query));
            id_of_all_connections[socket_id]->set_client_id(returned_value.first);
            result = std::move(returned_value.second);
            break;
        case RequestType::GET_BOARDS_INFO:
        default:
            result = std::move(ErrorJson{"An error occured"}.to_json());        
    }

    clientSocket->sendData(result.c_str());    
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