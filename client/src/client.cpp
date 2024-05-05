#include "client.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include "boardmenu.hpp"
#include "database.hpp"
#include "element_classes.hpp"
#include "jsonparser.hpp"
#include "logging.hpp"

Client::Client(QObject *parent)
    : QObject(parent),
      db(database::db_manager("local_boards", "postgres", "localhost", "")) {
    m_socket =
        new QWebSocket("Client", QWebSocketProtocol::VersionLatest, this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(
        m_socket, SIGNAL(textMessageReceived(const QString &)), this,
        SLOT(readData(const QString &))
    );
    connect(
        m_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this,
        SLOT(onSocketError(QAbstractSocket::SocketError))
    );

    m_local_id = db.authorize_user("default", "default");
}

void Client::write(std::string &data) {
    qDebug() << "Send:: " << data;
    m_socket->sendTextMessage(QString::fromStdString(data));
}

void Client::readData(const QString &data) {
    rDebug() << "Recieved: " << data;
    if (!(nlohmann::json::accept(data.toStdString()))) {
        qDebug() << "Cannot parse response";
        return;
    }
    nlohmann::json response = nlohmann::json::parse(data.toStdString());
    if (!response.contains("type") ||
        response["type"].type() != nlohmann::json::value_t::string) {
        qDebug() << "Invalid response structure";
        return;
    }
    std::string response_type = response["type"];
    if (response_type == "authorization") {
        if (response["response"] == "ok") {
            nlohmann::json avaliable_boards = response["boards"];
            prepare_remote_board_select_menu(avaliable_boards);
            m_connection_status = ConnectionStatus::Authorized;
            m_is_authorized = true;
            emit connectionStatusChanged();
            emit statusChanged();
        } else {
            m_connection_status = ConnectionStatus::Authentification_failed;
            emit connectionStatusChanged();
            qDebug() << "Wrong password!";
        }
    }
    if (response_type == "create-response") {
        [[maybe_unused]] quint32 board_id = response["board-id"];
        [[maybe_unused]] quint32 list_id = response["list-id"];
        [[maybe_unused]] quint32 card_id = response["card-id"];
        if (list_id == 0) {
            board new_board = parser::parse_board(response["object-json"]);
            m_board_menu->add_board(new_board);
        } else if (card_id == 0) {
            list new_list = parser::parse_list(response["object-json"]);
            m_current_board->create_list(new_list);
        } else {
            card new_card = parser::parse_card(response["object-json"]);
            m_current_board->create_card(list_id, new_card);
        }
    }
    if (response_type == "board") {
        const board loaded_board = parser::parse_board(response, m_user_id);
        m_loaded_boards[m_current_index] = new BoardModel(this, loaded_board);
        m_current_board = m_loaded_boards[m_current_index];
        emit boardChanged();
    }
}

void Client::load_board(int index) {
    quint32 board_id = m_board_menu->get_id(index);
    const board &board_base = db.select_board(board_id);
    m_current_board = new BoardModel(this, board_base);
    emit boardChanged();
}

void Client::prepare_local_board_select_menu() {
    m_mode = ClientMode::Local;
    emit modeChanged();
    if (m_local_menu) {
        m_board_menu = m_local_menu;
        emit menuChanged();
        return;
    }
    m_local_menu = new BoardMenu(this);
    QVector<board> avaliable_boards = db.get_group_boards(m_local_group_id);
    for (const auto &board : avaliable_boards) {
        m_local_menu->add_board(board);
    };

    m_board_menu = m_local_menu;
    emit menuChanged();
}

void Client::prepare_remote_board_select_menu(
    const nlohmann::json &avaliable_boards
) {
    m_mode = ClientMode::Remote;
    emit modeChanged();
    if (m_remote_menu) {
        m_board_menu = m_remote_menu;
        emit menuChanged();
        emit modeChanged();
        return;
    }
    m_remote_menu = new BoardMenu(this);
    for (auto board : avaliable_boards) {
        QString name = QString::fromStdString(board["name"]);
        QString description = QString::fromStdString(board["description"]);
        quint32 id = board["id"];
        m_remote_menu->create_board(name, description, id);
    }
    m_board_menu = m_remote_menu;
    emit menuChanged();
}

void Client::request_board(int index) {
    quint32 board_id = m_board_menu->get_id(index);
    m_current_index = index;
    if (m_mode == ClientMode::Local) {
        const board &requested_board = db.get_full_board(board_id);
        m_current_board = new BoardModel(this, requested_board);
        emit boardChanged();
    } else {
        std::string request = parser::board_request(board_id);
        write(request);
    }
}

void Client::create_list(QString name) {
    if (name == "") {
        name = "New list";
    }
    quint32 board_id = m_current_board->m_board_id;
    if (m_mode == ClientMode::Local) {
        quint32 list_id = db.insert_list(board_id, name, "");
        const list &new_list = db.select_list(list_id);
        m_current_board->create_list(new_list);
    } else {
        std::string request =
            parser::create_request("list", board_id, name, "", board_id);
        write(request);
    }
}

void Client::create_card(int list_index, QString name, QString description) {
    if (name == "") {
        name = "New card";
    }
    if (description == "") {
        description = "smth";
    }
    quint32 board_id = m_current_board->m_board_id;
    quint32 list_id = m_current_board->get_list_id(list_index);
    if (m_mode == ClientMode::Local) {
        quint32 card_id = db.insert_card(list_id, name, description);
        const card &new_card = db.select_card(card_id);
        m_current_board->create_card(list_index, new_card);
    } else {
        std::string request = parser::create_request(
            "card", list_id, name, description, board_id, list_id
        );
        write(request);
    }
}

void Client::create_board(QString name, QString description) {
    if (name == "") {
        name = "New board";
    }
    if (description == "") {
        description = "smth";
    }

    if (m_mode == ClientMode::Local) {
        quint32 board_id = db.insert_board(m_local_group_id, name, description);
        m_board_menu->create_board(name, description, board_id);
    } else {
        std::string request =
            parser::create_request("board", m_user_id, name, description);
        write(request);
    }
}

void Client::delete_list(int list_index) {
    quint32 list_id = m_current_board->get_list_id(list_index);
    if (m_mode == ClientMode::Local) {
        db.delete_command("list_signature", list_id);
    } else {
        std::string request = parser::delete_request(list_id, "list");
        write(request);
    }
    m_current_board->delete_list(list_index);
}

void Client::delete_card(int list_index, int card_index) {
    [[maybe_unused]] quint32 list_id = m_current_board->get_list_id(list_index);
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    if (m_mode == ClientMode::Local) {
        db.delete_command("card_signature", card_id);
    } else {
        std::string request = parser::delete_request(card_id, "card");
        write(request);
    }
    m_current_board->delete_card(list_index, card_index);
}

void Client::onConnected() {
    qDebug() << "Connected!";
    std::string login_request = parser::login_request(m_username, m_password);
    write(login_request);
}

void Client::onSocketError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);
    qDebug() << "Connection error!";
    m_connection_status = ConnectionStatus::Unable_to_connect;
    emit connectionStatusChanged();
}

void Client::login(
    QString username,
    QString password,
    QString server_ip,
    QString server_port
) {
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        if (server_ip == m_server_ip &&
            server_port.toUShort() == m_server_port && m_is_authorized) {
            return;
        }
        m_socket->close();
    }
    m_server_ip = server_ip;
    m_server_port = server_port.toUShort();
    m_username = username;
    m_password = password;
    m_is_authorized = false;
    emit statusChanged();
    QUrl server_url;
    qDebug() << "Trying to connect!";
    server_url.setScheme(QString::fromStdString("ws"));
    server_url.setHost(m_server_ip);
    server_url.setPort(m_server_port);
    m_socket->open(server_url);
}

void Client::update_card_name(int list_index, int card_index, QString name) {
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    if (m_mode == ClientMode::Local &&
        db.update_command("card_signature", "name", name, card_id)) {
        m_current_board->update_card_name(list_index, card_index, name);
    } else {
        std::string request =
            parser::update_request("card", card_id, "name", name);
        write(request);
        m_current_board->update_card_name(
            list_index, card_index, name
        );  // TODO
    }
}

void Client::update_card_description(
    int list_index,
    int card_index,
    QString description
) {
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    if (m_mode == ClientMode::Local &&
        db.update_command(
            "card_signature", "description", description, card_id
        )) {
        m_current_board->update_card_description(
            list_index, card_index, description
        );
    } else {
        std::string request =
            parser::update_request("card", card_id, "description", description);
        write(request);
        m_current_board->update_card_description(
            list_index, card_index, description
        );
    }
}

void Client::update_list_name(int list_index, QString name) {
    quint32 list_id = m_current_board->get_list_id(list_index);
    if (m_mode == ClientMode::Local &&
        db.update_command("list_signature", "name", name, list_id)) {
        m_current_board->update_list_name(list_index, name);
    } else {
        std::string request =
            parser::update_request("list", list_id, "name", name);
        write(request);
        m_current_board->update_list_name(list_index, name);
    }
}

QString Client::get_current_board_name() {
    return m_current_board->m_name;
}

void Client::connect_board(quint32 board_id) {
    std::string request = parser::connect_to_board_request(board_id, m_user_id);
    write(request);
}

void Client::move(int from_card, int to_card, int from_list, int to_list) {
    m_current_board->move(from_card, to_card, from_list, to_list);
    if (m_mode == ClientMode::Local) {
        qDebug() << "DB UPDATE";
    } else {
        std::string request =
            parser::move_request(from_card, to_card, from_list, to_list);
        write(request);
    }
}
