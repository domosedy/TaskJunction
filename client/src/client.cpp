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
    m_board_menu = new BoardMenu(this);

    m_local_id = db.authorize_user("default", "default");
}

void Client::write(std::string &data) {
    qDebug() << "Send:: " << data;
    m_socket->sendTextMessage(QString::fromStdString(data));
}

void Client::readData(const QString &data) {
    qDebug() << "Recieved: " << data;
    if (!(nlohmann::json::accept(data.toStdString()))) {
        qDebug() << "Cannot parse response";
        return;
    }
    nlohmann::json response = nlohmann::json::parse(data.toStdString());
    std::string response_type = response["type"];
    if (response_type == "authorization") {
        if (response["response"] == "ok") {
            nlohmann::json avaliable_boards = response["boards"];
            load_remote_boards(avaliable_boards);
            m_connection_status = ConnectionStatus::Authorized;
            emit connectionStatusChanged();
        } else {
            m_connection_status = ConnectionStatus::Authentification_failed;
            emit connectionStatusChanged();
            qDebug() << "Wrong password!";
        }
    }
    if (response_type == "create-response") {
        quint32 board_id = response["board-id"];
        quint32 list_id = response["list-id"];
        quint32 card_id = response["card-id"];
        if (list_id == 0) {
            board new_board =
                parser::parse_board(response["object-json"], m_user_id);
            new_board.m_is_remote = true;
            m_board_menu->add_board(new_board);
        } else if (card_id == 0) {
            list new_list =
                parser::parse_list(response["object-json"], board_id);
            m_current_board->create_list(new_list);
        } else {
            card new_card =
                parser::parse_card(response["object-json"], list_id);
            m_current_board->create_card(list_id, new_card);
        }
    }
    if (response_type == "board") {
        const board loaded_board = parser::parse_board(response, m_user_id);
        m_current_board = m_board_menu->load(m_current_index, loaded_board);
        emit boardChanged();
    }
}

void Client::load_local_boards() {
    if (m_board_menu->is_local_loaded) {
        return;
    }
    QVector<board> avaliable_boards = db.get_user_boards(m_local_id);
    for (const auto &board : avaliable_boards) {
        m_board_menu->add_board(board);
    };
    m_board_menu->is_local_loaded = true;
}

void Client::load_remote_boards(const nlohmann::json &avaliable_boards) {
    for (auto board : avaliable_boards) {
        QString name = QString::fromStdString(board["name"]);
        QString description = QString::fromStdString(board["description"]);
        quint32 id = board["id"];
        m_board_menu->add_board(name, description, id, m_user_id);
    }
}

void Client::load_board(int index) {
    const auto &[board_id, is_remote] = m_board_menu->get_info(index);
    m_current_index = index;
    if (!is_remote) {
        const board &loaded_board = db.get_full_board(board_id);
        m_current_board = m_board_menu->load(index, loaded_board);
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
    if (!m_current_board->m_is_remote) {
        quint32 list_id = db.insert_list(board_id, name, "");
        const list &new_list = db.select_list(list_id);
        m_current_board->create_list(new_list);
    } else {
        std::string request =
            parser::create_request("list", board_id, name, "", board_id);
        write(request);
    }
}

void Client::create_tag(int list_index, int card_index, QString name) {
    if (name == "") {
        name = "lol";
    }    
    //quint32 board_id = m_current_board->m_board_id;
    //quint32 list_id = m_current_board->get_list_id(list_index);
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    if (!m_current_board->m_is_remote) {
        quint32 tag_id = db.insert_tag(name);
        db.add_tag_to_card(card_id, tag_id);
        const tag &new_tag = db.select_tag(tag_id);
        m_current_board->create_tag(list_index, card_index, new_tag);
    } else {
        std::string request = parser::create_request("card", card_id, name, "");
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
    if (!m_current_board->m_is_remote) {
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

void Client::add_board(QString name, QString description, QString type) {
    if (name == "") {
        name = "New board";
    }
    if (description == "") {
        description = "smth";
    }

    if (type == "Local") {
        quint32 board_id = db.insert_board(m_local_id, name, description);
        m_board_menu->add_board(name, description, board_id, m_local_id);
    } else {
        std::string request =
            parser::create_request("board", m_user_id, name, description);
        write(request);
    }
}

void Client::delete_board(int board_index) {
    const auto &[board_id, is_remote] =
        m_board_menu->delete_board(board_index);  // TODO make afterwards?
    qDebug() << board_id;
    if (!is_remote) {
        db.delete_board(board_id);
    } else {
        std::string request = parser::delete_request(board_id, "board");
        write(request);
    }
}

void Client::delete_list(int list_index) {
    quint32 list_id = m_current_board->get_list_id(list_index);
    if (!m_current_board->m_is_remote) {
        db.delete_list(list_id);
    } else {
        std::string request = parser::delete_request(list_id, "list");
        write(request);
    }
    m_current_board->delete_list(list_index);
}

void Client::delete_card(int list_index, int card_index) {
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    if (!m_current_board->m_is_remote) {
        db.delete_card(card_id);
    } else {
        std::string request = parser::delete_request(card_id, "card");
        write(request);
    }
    m_current_board->delete_card(list_index, card_index);
}

void Client::delete_tag(int list_index, int card_index, int tag_index) {
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    quint32 tag_id = m_current_board->get_tag_id(list_index, card_index, tag_index);    
    if (!m_current_board->m_is_remote) {
        db.delete_tag_from_card(card_id, tag_id);
        m_current_board->delete_tag(list_index, card_index, tag_index);
    } else {
        std::string request = parser::delete_request(tag_id, "tag"); // PASS CARD_ID 
        write(request);        
    }
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
        m_board_menu->unload_remote_boards();
        m_socket->close();
    }
    m_username = username;
    m_password = password;
    QUrl server_url;
    qDebug() << "Connecting...";
    server_url.setScheme(QString::fromStdString("ws"));
    server_url.setHost(server_ip);
    server_url.setPort(server_port.toUShort());
    m_socket->open(server_url);
}

void Client::update_card(
    int list_index,
    int card_index,
    QString field,
    QString value
) {
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    if (!m_current_board->m_is_remote &&
        db.update_command("card_signature", field, value, card_id)) {
        m_current_board->update_card(list_index, card_index, field, value);
    } else {
        std::string request =
            parser::update_request("card", card_id, field, value);
        write(request);
        m_current_board->update_card(
            list_index, card_index, field, value
        );  // make afterwards?
    }
}

void Client::update_list(int list_index, QString name) {
    quint32 list_id = m_current_board->get_list_id(list_index);
    if (!m_current_board->m_is_remote &&
        db.update_command("list_signature", "name", name, list_id)) {
        m_current_board->update_list(list_index, name);
    } else {
        std::string request =
            parser::update_request("list", list_id, "name", name);
        write(request);
        m_current_board->update_list(list_index, name);
    }
}

void Client::update_board(int board_index, QString field, QString value) {
    const auto &[board_id, is_remote] = m_board_menu->get_info(board_index);
    if (!is_remote &&
        db.update_command("board_signature", field, value, board_id)) {
        m_board_menu->update_board(board_index, field, value);
    } else {
        std::string request =
            parser::update_request("board", board_id, field, value);
        write(request);
        m_board_menu->update_board(
            board_index, field, value
        );  // make afterwards?
    }
}

QString Client::get_current_board_name() {
    return m_current_board->m_name;
}

void Client::move(int from_card, int to_card, int from_list, int to_list) {
    m_current_board->move(from_card, to_card, from_list, to_list);
    quint32 to_list_id = m_current_board->get_list_id(to_list);
    quint32 card_id = m_current_board->get_card_id(from_list, from_card);
    if (!m_current_board->m_is_remote) {
        db.move_card(card_id, to_list_id, to_card);
    } else {
        std::string request =
            parser::move_request(card_id, to_list_id, to_card);
        write(request);
    }
}

void Client::connect_board(QString link) {
    Q_UNUSED(link);
    qDebug() << "Still nothing here";
}
