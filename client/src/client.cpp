#include "client.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include "boardmenu.hpp"
#include "database.hpp"
#include "element_classes.hpp"
#include "jsonparser.hpp"

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
    connect(m_socket, SIGNAL(sslErrors(const QList<QSslError> &)),
            this, SLOT(onSslErrors(const QList<QSslError> &)));
    m_board_menu = new BoardMenu(this);
}

void Client::onSslErrors(const QList<QSslError> &errors) {
    qDebug() << "Errors: " << errors;
    m_socket->ignoreSslErrors(errors);
}

void Client::write(std::string &data) {
    qDebug() << "Send:: " << data;
    m_socket->sendTextMessage(QString::fromStdString(data));
}

void Client::readData(const QString &data) {
    qDebug() << "Recieved:: " << data;
    if (!(nlohmann::json::accept(data.toStdString()))) {
        qDebug() << "Cannot parse response";
        return;
    }
    nlohmann::json response = nlohmann::json::parse(data.toStdString());
    std::string response_type = response["type"];
    if (response_type == "authorization") {
        if (response["response"] == "ok") {
            const nlohmann::json avaliable_boards = response["boards"];
            load_remote_boards(avaliable_boards);
            m_connection_status = ConnectionStatus::Authorized;
            emit connectionStatusChanged();
        } else {
            m_connection_status = ConnectionStatus::Authentification_failed;
            emit connectionStatusChanged();
            qDebug() << "Wrong password!";
        }
    }
    if (response_type == "create") {
        quint32 board_id = response["board-id"];
        quint32 list_id = response["list-id"];
        quint32 card_id = response["card-id"];
        quint32 tag_id = response["tag-id"];

        if (m_board_menu->is_board_loaded(board_id) && list_id == 0) {
            return;
        }
        if (tag_id != 0) {
            tag new_tag = parser::parse_tag(response["object-json"]);
            m_board_menu->create_tag(board_id, list_id, card_id, new_tag);
            return;
        }
        if (card_id != 0) {
            card new_card =
                parser::parse_card(response["object-json"], list_id);
            m_board_menu->create_card(board_id, list_id, new_card);
            return;
        }
        if (list_id != 0) {
            list new_list =
                parser::parse_list(response["object-json"], board_id);
            m_board_menu->create_list(board_id, new_list);
            return;
        }
        board new_board =
            parser::parse_board(response["object-json"], m_user_id);
        new_board.m_is_remote = true;
        m_board_menu->create_board(new_board);
    }
    if (response_type == "board") {
        board loaded_board = parser::parse_board(response, m_user_id);
        loaded_board.m_is_remote = true;
        m_current_board = m_board_menu->load(loaded_board);
        emit boardChanged();
    }
    if (response_type == "delete") {
        quint32 board_id = response["board-id"];
        quint32 list_id = response["list-id"];
        quint32 card_id = response["card-id"];
        quint32 tag_id = response["tag-id"];
        if (!m_board_menu->is_board_loaded(board_id)) {
            return;
        }
        m_board_menu->delete_command(board_id, list_id, card_id, tag_id);
    }
    if (response_type == "update") {
        quint32 board_id = response["board-id"];
        quint32 list_id = response["list-id"];
        quint32 card_id = response["card-id"];
        QString new_value = QString::fromStdString(response["new-value"]);
        QString field = QString::fromStdString(response["field"]);

        if (!m_board_menu->is_board_loaded(board_id)) {
            return;
        }
        m_board_menu->update_command(
            board_id, list_id, card_id, field, new_value
        );
    }
    if (response_type == "move") {
        quint32 board_id = response["board-id"];
        quint32 old_list_id = response["old-list-id"];
        quint32 new_list_id = response["new-list-id"];
        quint32 card_id = response["card-id"];
        int new_index = response["new-index"];

        if (!m_board_menu->is_board_loaded(board_id)) {
            return;
        }

        m_board_menu->move_command(
            board_id, old_list_id, new_list_id, card_id, new_index
        );
    }
    if (response_type == "connect" && response["status"] == "ok") {
        board new_board = parser::parse_board(response["board"], m_user_id);
        new_board.m_is_remote = true;
        m_board_menu->create_board(new_board);
    }
}

void Client::load_local_boards() {
    if (m_board_menu->is_local_loaded) {
        return;
    }
    QVector<board> avaliable_boards = db.get_user_boards(m_local_id);
    for (const auto &board : avaliable_boards) {
        m_board_menu->create_board(board);
    };
    m_board_menu->is_local_loaded = true;
}

void Client::load_remote_boards(const nlohmann::json &avaliable_boards) {
    if (!m_board_menu->is_local_loaded) {
        load_local_boards();
    }
    for (auto board : avaliable_boards) {
        QString name = QString::fromStdString(board["name"]);
        QString description = QString::fromStdString(board["description"]);
        quint32 id = board["id"];
        QString link = QString::fromStdString(board["link"]);
        m_board_menu->create_board(
            name, description, id, m_user_id, link, true
        );
    }
}

void Client::load_board(int index) {
    const auto &[board_id, is_remote] = m_board_menu->get_info(index);
    if (!is_remote) {
        const board &loaded_board = db.get_full_board(board_id);
        m_current_board = m_board_menu->load(loaded_board);
        emit boardChanged();
    } else {
        m_current_board = nullptr;
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
    quint32 board_id = m_current_board->m_board_id;
    quint32 list_id = m_current_board->get_list_id(list_index);
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    if (!m_current_board->m_is_remote) {
        quint32 tag_id = db.insert_tag(name);
        db.add_tag_to_card(card_id, tag_id);
        const tag &new_tag = db.select_tag(tag_id);
        m_current_board->create_tag(list_index, card_index, new_tag);
    } else {
        std::string request = parser::create_request(
            "tag", card_id, name, "", board_id, list_id, card_id
        );
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

void Client::create_board(QString name, QString description, bool is_remote) {
    if (name == "") {
        name = "New board";
    }
    if (description == "") {
        description = "smth";
    }

    if (!is_remote) {
        quint32 board_id = db.insert_board(m_local_id, name, description, "");
        m_board_menu->create_board(name, description, board_id, m_local_id);
    } else {
        std::string request =
            parser::create_request("board", m_user_id, name, description);
        write(request);
    }
}

void Client::delete_board(int board_index) {
    const auto &[board_id, is_remote] = m_board_menu->delete_board(board_index);
    qDebug() << board_id;
    if (!is_remote) {
        db.delete_board(board_id);
    } else {
        std::string request =
            parser::delete_request(board_id, "board", board_id);
        write(request);
    }
}

void Client::delete_list(int list_index) {
    quint32 list_id = m_current_board->get_list_id(list_index);
    if (!m_current_board->m_is_remote) {
        db.delete_list(list_id);
        m_current_board->delete_command(list_index, -1, -1);
    } else {
        std::string request = parser::delete_request(
            list_id, "list", m_current_board->m_board_id, list_id
        );
        write(request);
    }
}

void Client::delete_card(int list_index, int card_index) {
    quint32 list_id = m_current_board->get_list_id(list_index);
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    if (!m_current_board->m_is_remote) {
        db.delete_card(card_id);
        m_current_board->delete_command(list_index, card_index, -1);
    } else {
        std::string request = parser::delete_request(
            card_id, "card", m_current_board->m_board_id, list_id, card_id
        );
        write(request);
    }
}

void Client::delete_tag(int list_index, int card_index, int tag_index) {
    quint32 list_id = m_current_board->get_list_id(list_index);
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    quint32 tag_id =
        m_current_board->get_tag_id(list_index, card_index, tag_index);
    if (!m_current_board->m_is_remote) {
        db.delete_tag_from_card(card_id, tag_id);
        m_current_board->delete_command(list_index, card_index, tag_index);
    } else {
        std::string request = parser::delete_request(
            tag_id, "tag", m_current_board->m_board_id, list_id, card_id, tag_id
        );  // TODO FIX
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
        m_socket->close();
    }
    m_board_menu->unload_remote_boards();
    m_username = username;
    m_password = password;
    QUrl server_url;
    qDebug() << "Connecting...";
    server_url.setScheme(QString::fromStdString("wss"));
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
    quint32 list_id = m_current_board->get_list_id(list_index);
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    if (!m_current_board->m_is_remote &&
        db.update_command("card_signature", field, value, card_id)) {
        m_current_board->update_command(list_index, card_index, field, value);
    } else {
        std::string request = parser::update_request(
            "card", card_id, field, value, m_current_board->m_board_id, list_id,
            card_id
        );
        write(request);
    }
}

void Client::update_list(int list_index, QString name) {
    quint32 list_id = m_current_board->get_list_id(list_index);
    if (!m_current_board->m_is_remote &&
        db.update_command("list_signature", "name", name, list_id)) {
        m_current_board->update_command(list_index, -1, "name", name);
    } else {
        std::string request = parser::update_request(
            "list", list_id, "name", name, m_current_board->m_board_id, list_id
        );
        write(request);
    }
}

void Client::update_board(int board_index, QString field, QString value) {
    const auto &[board_id, is_remote] = m_board_menu->get_info(board_index);
    if (!is_remote &&
        db.update_command("board_signature", field, value, board_id)) {
        m_board_menu->update_board(board_index, field, value);
    } else {
        std::string request =
            parser::update_request("board", board_id, field, value, board_id);
        write(request);
    }
}

QString Client::get_current_board_name() {
    if (m_current_board) {
        return m_current_board->m_name;
    } else {
        return "";
    }
}

void Client::move(int from_card, int to_card, int from_list, int to_list) {
    qDebug() << "moving from: (" << from_list << ':' << from_card << ") to ("
             << to_list << ":" << to_card << ')';
    quint32 list_id = m_current_board->get_list_id(from_list);
    quint32 to_list_id = m_current_board->get_list_id(to_list);
    quint32 card_id = m_current_board->get_card_id(from_list, from_card);
    qDebug() << "move(" << card_id << to_list_id << to_card << ")";
    if (!m_current_board->m_is_remote) {
        db.move_card(card_id, to_list_id, to_card + 1);
        m_current_board->move(from_card, to_card, from_list, to_list);
    } else {
        std::string request = parser::move_request(
            to_list_id, to_card, m_current_board->m_board_id, list_id, card_id
        );
        write(request);
    }
}

bool Client::is_filtered(int list_index, int card_index) const {
    if (m_filter.isEmpty()) {
        return true;
    }
    quint32 card_id = m_current_board->get_card_id(list_index, card_index);
    return m_filtered_cards.find(card_id) != m_filtered_cards.end();
}

void Client::set_filter(QString filter, bool is_all) {
    if (filter == "") {
        m_filtered_cards.clear();
        m_filter.clear();
        return;
    }
    m_filter = filter.split(", ");  // todo make better?
    if (is_all) {
        m_filtered_cards =
            db.all_filter_cards(m_current_board->m_board_id, m_filter);
    } else {
        m_filtered_cards =
            db.any_filter_cards(m_current_board->m_board_id, m_filter);
    }
}

void Client::connect_board(QString link) {
    std::string request = parser::connect_to_board_request(link);
    write(request);
}

void Client::copy_board(int board_index) {
    const nlohmann::json board_data_json =
        m_board_menu->board_to_json(board_index);
    auto [id, is_remote] = m_board_menu->get_info(board_index);
    if (!is_remote) {
        std::string request = parser::upload_request(board_data_json);
        write(request);
    } else {
        board board_data = parser::parse_board(board_data_json, m_local_id);
        board_data.m_name.append(" copy");
        board copy_board = db.copy_board(board_data, m_local_id);
        m_board_menu->create_board(copy_board);
    }
}
