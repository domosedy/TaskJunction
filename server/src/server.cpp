#include "server.hpp"
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <functional>
#include <memory>
#include "client_socket.hpp"
#include "element_classes.hpp"
#include "hashes.hpp"
#include "logging.hpp"
#include "query.hpp"
// #include "datas.hpp"

enum class RequestType {
    ERROR,
    CREATE,
    DELETE,
    UPDATE,
    LOGIN,
    GET_BOARDS_INFO,
    MOVE,
    ACCESS,
    UPLOAD
};

// How to emit when my connection is not authorized?
Server::Server(quint16 port)
    : port(port), db("postgres", "postgres", "localhost", "") {
    server = new QWebSocketServer(
        QStringLiteral("Task Server"), QWebSocketServer::SecureMode, this
    );

    QSslConfiguration sslConfiguration;
    QFile certFile(QStringLiteral(":/localhost.cert"));
    QFile keyFile(QStringLiteral(":/localhost.key"));
    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    server->setSslConfiguration(sslConfiguration);

    if (!server->listen(QHostAddress::Any, port)) {
        rDebug() << QObject::tr("Unable to start server: %1")
                        .arg(server->errorString());
        std::exit(1);
    }

    rDebug() << "readed";
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(server, &QWebSocketServer::sslErrors, this, &Server::onSslErrors);
}

void Server::onSslErrors(const QList<QSslError> &) {
    rDebug() << "Errors in Ssl";
}

Server::~Server() {
    server->close();
    qDeleteAll(
        unauthorized_connections.begin(), unauthorized_connections.end()
    );
    for (auto &connection : authorized_connections) {
        delete connection;
    }
}

void Server::newConnection() {
    QWebSocket *clientSocket = server->nextPendingConnection();
    ClientSocket *clientSock = new ClientSocket(clientSocket, 0);

    connect(
        clientSock, SIGNAL(request_to_database(uint, query_type)), this,
        SLOT(execute_query(uint, query_type))
    );

    connect(clientSock, SIGNAL(disconnected()), this, SLOT(removeConnection()));

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
            } else if constexpr (std::is_same_v<T, move_query>) {
                return RequestType::MOVE;
            } else if constexpr (std::is_same_v<T, access_to_board>) {
                return RequestType::ACCESS;
            } else if constexpr (std::is_same_v<T, copy_board_query>) {
                return RequestType::UPLOAD;
            }
            return RequestType::ERROR;
        },
        query
    );

    ReturnedValue executed_result;

    if (result_code == RequestType::LOGIN) {
        auto res = execute_login_query(std::get<login_query>(query));
        if (res.second != 0) {
            clientSocket->set_client_id(res.second);
            unauthorized_connections.removeAll(clientSocket);
            authorized_connections[res.second] = clientSocket;
        }
        auto result = res.first.toStdString();
        clientSocket->sendData(result.c_str());
        return;
    } else if (result_code == RequestType::UPDATE) {
        executed_result =
            execute_update_query(std::get<update_query>(query), client_id);
    } else if (result_code == RequestType::CREATE) {
        executed_result =
            execute_create_query(std::get<create_query>(query), client_id);
    } else if (result_code == RequestType::DELETE) {
        executed_result =
            execute_delete_query(std::get<delete_query>(query), client_id);
    } else if (result_code == RequestType::GET_BOARDS_INFO) {
        executed_result = execute_get_query(
            std::get<get_boards_info_query>(query), client_id
        );
        clientSocket->sendData(executed_result.jsoned_object);
        return;
    } else if (result_code == RequestType::MOVE) {
        executed_result =
            execute_move_query(std::get<move_query>(query), client_id);
    } else if (result_code == RequestType::ACCESS) {
        auto res =
            execute_access_query(std::get<access_to_board>(query), client_id);
        auto result = res.jsoned_object;
        clientSocket->sendData(result);
        return;
    } else if (result_code == RequestType::UPLOAD) {
        if (client_id == 0) {
            clientSocket->sendData(
                error{"You are not authorized"}.to_json().c_str()
            );
            return;
        }

        auto res =
            execute_upload_query(std::get<copy_board_query>(query), client_id);
        clientSocket->sendData(res.jsoned_object);
        return;
    } else {
        executed_result = {
            false, 1, error{"Bad request format"}.to_json().c_str()
        };
    }

    if (!executed_result.exit_code) {
        clientSocket->sendData(executed_result.jsoned_object);
        return;
    }

    rDebug() << "Finding users in this board_id:";
    for (auto &user_id : db.get_board_user_ids(executed_result.board_id)) {
        auto it = authorized_connections.find(user_id);
        rDebug() << user_id;
        if (it != authorized_connections.end()) {
            rDebug() << it.value()->get_client_id();
            it.value()->sendData(executed_result.jsoned_object);
        }
    }
}

ReturnedValue
Server::execute_update_query(const update_query &query, quint32 id) {
    if (!db.check_user_rights(id, query.all_id.board_id)) {
        return ReturnedValue{
            false, 0, error{"You have no rights"}.to_json().c_str()
        };
    }

    auto result = db.update_command(
        (query.updated_type + "_signature").c_str(), query.value_name.c_str(),
        query.new_value.c_str(), query.value_id
    );

    update_response response{
        query.all_id, query.new_value.c_str(), query.value_name.c_str()
    };

    return result
               ? ReturnedValue{true, query.all_id.board_id, response.to_json().c_str()}
               : ReturnedValue{
                     false, 0, error{"An error occured"}.to_json().c_str()
                 };
}

ReturnedValue
Server::execute_delete_query(const delete_query &query, quint32 id) {
    if (!db.check_user_rights(id, query.all_id.board_id)) {
        return ReturnedValue{
            false, 0, error{"You have no rights"}.to_json().c_str()
        };
    }

    bool result = false;

    if (query.value_type == "board") {
        result = db.delete_board(query.value_id);
    } else if (query.value_type == "list") {
        result = db.delete_list(query.value_id);
    } else if (query.value_type == "card") {
        result = db.delete_card(query.value_id);
    } else if (query.value_type == "tag") {
        result = db.delete_tag_from_card(query.all_id.card_id, query.value_id);
    }

    delete_response response{query.all_id};

    return result
               ? ReturnedValue{true, query.all_id.board_id, response.to_json().c_str()}
               : ReturnedValue{
                     false, 0, error{"An error occured"}.to_json().c_str()
                 };
}

ReturnedValue
Server::execute_create_query(const create_query &query, quint32 user_id) {
    if (user_id == 0) {
        return ReturnedValue{
            false, 0, error{"You are not authorized"}.to_json().c_str()
        };
    }

    quint32 result = 0;

    rDebug() << user_id;
    rDebug() << query.value_type.c_str();

    std::string jsoned_object = "";
    quint32 board_id = query.all_id.board_id;
    auto all_ids = query.all_id;

    if (query.value_type == "board") {
        rDebug() << "Hello from board";
        rDebug() << query.value_name.c_str();
        rDebug() << query.value_description.c_str();

        result = db.insert_board(
            user_id, query.value_name.c_str(), query.value_description.c_str(),
            generate_string()
        );

        board_id = result;
        if (result != 0) {
            all_ids.board_id = result;
            auto res = db.select_board(result);
            res.m_link = code_string(res.m_link, result);
            // QString::number(result) + " " + res.m_link;
            jsoned_object = res.to_json();
        }
    } else if (query.value_type == "list") {
        if (!db.check_user_rights(user_id, query.all_id.board_id)) {
            return ReturnedValue{
                false, 0,
                error{"You don't have rights on this group"}.to_json().c_str()
            };
        }
        result = db.insert_list(
            query.parent_id, query.value_name.c_str(),
            query.value_description.c_str()
        );

        if (result != 0) {
            all_ids.list_id = result;
            jsoned_object = db.select_list(result).to_json();
        }
    } else if (query.value_type == "card") {
        if (!db.check_user_rights(user_id, query.all_id.board_id)) {
            return ReturnedValue{
                false, 0,
                error{"You don't have rights on this group"}.to_json().c_str()
            };
        }

        result = db.insert_card(
            query.parent_id, query.value_name.c_str(),
            query.value_description.c_str()
        );

        if (result != 0) {
            all_ids.card_id = result;
            jsoned_object = db.select_card(result).to_json();
        }
    } else if (query.value_type == "tag") {
        if (!db.check_user_rights(user_id, query.all_id.board_id)) {
            return ReturnedValue{
                false, 0,
                error{"You don't have rights on this group"}.to_json().c_str()
            };
        }

        result = db.insert_tag(query.value_name.c_str());
        if (result != 0 && db.add_tag_to_card(query.all_id.card_id, result)) {
            all_ids.tag_id = result;
            jsoned_object = db.select_tag(result).to_json().c_str();
        } else {
            result = 0;
        }
    }

    create_response response{
        all_ids, result, query.value_type.c_str(), jsoned_object
    };
    return result > 0
               ? ReturnedValue{true, board_id, response.to_json().c_str()}
               : ReturnedValue{
                     false, 0, error{"An error occured"}.to_json().c_str()
                 };
}

std::pair<QString, quint32> Server::execute_login_query(const login_query &query
) {
    auto id =
        db.authorize_user(query.user_name.c_str(), query.password.c_str());
    login response{id != 0};

    rDebug() << "user id is: " << id;
    if (response.m_response) {
        for (auto board : db.get_user_boards(id)) {
            board.m_link = code_string(board.m_link, board.m_board_id);
            //  QString::number(board.m_board_id) + " " + board.m_link;
            rDebug() << "This is board link " << board.m_link
                     << board.m_board_id;
            response.m_boards.push_back(board);
        }

        rDebug() << response.m_boards.size();
    }

    return {response.to_json().c_str(), id};
}

ReturnedValue Server::execute_upload_query(copy_board_query query, quint32 id) {
    if (query.board_to_copy.m_board_id == 0) {
        return {false, 0, error{"Bad format of data"}.to_json().c_str()};
    }

    query.board_to_copy.m_link = generate_string();
    board result = db.copy_board(query.board_to_copy, id);
    return {true, result.m_board_id, result.to_json().c_str()};
}

ReturnedValue Server::execute_move_query(const move_query &query, quint32 id) {
    if (!db.check_user_rights(id, query.all_id.board_id)) {
        return ReturnedValue{
            false, 0, error{"You have no rights"}.to_json().c_str()
        };
    }

    auto result = db.move_card(
        query.all_id.card_id, query.new_list_id,
        static_cast<int>(query.new_index) - 1
    );
    move_response response{
        query.all_id, query.old_list_id, query.new_list_id, query.new_index
    };
    rDebug() << query.all_id.card_id << ' ' << query.new_list_id << ' '
             << query.new_index << ' ' << result;
    return result
               ? ReturnedValue{true, query.all_id.board_id, response.to_json().c_str()}
               : ReturnedValue{
                     false, query.all_id.board_id,
                     error{"Error in moving"}.to_json().c_str()
                 };
}

ReturnedValue
Server::execute_access_query(const access_to_board &query, quint32 id) {
    auto board_link = db.select_board(query.board_id).m_link.toStdString();

    if (board_link == query.link) {
        auto board = db.get_full_board(query.board_id);
        board.m_link = code_string(board.m_link, board.m_board_id);
        // rDebug() << board.m_link;
        return ReturnedValue{true, query.board_id, board.to_json().c_str()};
    }

    return ReturnedValue{
        false, query.board_id, error{"Link is not valid"}.to_json().c_str()
    };
}

ReturnedValue
Server::execute_get_query(const get_boards_info_query &query, quint32 id) {
    if (!db.check_user_rights(id, query.id)) {
        rDebug() << id << " " << query.id;
        return ReturnedValue{
            false, 0, error{"You have no rights"}.to_json().c_str()
        };
    }

    rDebug() << "Get query";
    board result = db.get_full_board(query.id);
    rDebug() << query.id;

    result.m_link = code_string(result.m_link, query.id);
    rDebug() << result.m_link;

    return ReturnedValue{true, 0, result.to_json().c_str()};
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