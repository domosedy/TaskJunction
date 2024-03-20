#include "client.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include "boardmenu.hpp"
#include "database.hpp"
#include "element_classes.hpp"
#include "json_parser.hpp"
#include "logging.hpp"

Client::Client(QObject *parent)
    : QObject(parent),
      db(database::db_manager("postgres", "postgres", "localhost", "1")) {
    m_socket = new QTcpSocket(this);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

void Client::write(std::string &data) {
    QByteArray msg = data.c_str();
    QByteArray sz;
    QDataStream sendStream(&sz, QIODevice::WriteOnly);
    quint32 size = msg.size();
    sendStream << size;
    rDebug() << size;
    m_socket->write(sz + msg);
}

void Client::readData() {
    QDataStream in(m_socket);
    if (in.status() == QDataStream::Ok) {
        QDataStream size_data = m_socket->read(2);
        quint32 size;
        size_data >> size;
        QByteArray data = m_socket->read(size);
        qDebug() << data.toStdString().c_str();
    } else {
        qDebug() << "Error";
    }
}

void Client::parse_response(const QString &data) {
    if (nlohmann::json::accept(data.toStdString())) {
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
        if (!response.contains("response")) {
            qDebug() << "Invalid response structure";
        }
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
        // validate
        [[maybe_unused]] quint32 board_id = response["board-id"];
        [[maybe_unused]] quint32 list_id = response["list-id"];
        [[maybe_unused]] quint32 card_id = response["card-id"];
        if (list_id == 0) {
            board new_board = parser::parse_board(response["object"]);
            m_board_menu->add_board(new_board);
        }
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
    connect(
        m_local_menu, SIGNAL(load_boaard(int)), this, SLOT(load_board(int))
    );
    QVector<board> avaliable_boards = db.get_user_boards(m_local_id);
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
    m_board_menu = m_local_menu;
    emit menuChanged();
}

void Client::request_board(int index) {
    quint32 board_id = m_board_menu->get_id(index);
    if (m_mode == ClientMode::Local) {
        const board &requested_board = db.select_board(board_id);
        m_current_board = new BoardModel(this, requested_board);
        emit boardChanged();
    } else {
        std::string request = parser::board_request(board_id);
        write(request);
    }
}

void Client::create_list(QString &name) {
    if (m_mode == ClientMode::Local) {
        quint32 board_id = m_current_board->m_board_id;
        quint32 list_id = db.insert_list(board_id, name, "");
        const list &new_list = db.select_list(list_id);
        m_current_board->create_list(new_list);
    } else {
        std::string request =
            parser::create_list_request(m_current_board->m_board_id, name);
        write(request);
    }
}

void Client::create_card(int list_index, QString &name, QString &description) {
    if (m_mode == ClientMode::Local) {
        quint32 list_id = m_current_board->get_list_id(list_index);
        quint32 card_id = db.insert_card(list_id, name, description);
        const card &new_card = db.select_card(card_id);
        m_current_board->create_card(list_index, new_card);
    } else {
        std::string request =
            parser::create_card_request(list_index, name, description);
        write(request);
    }
}

void Client::create_board(QString &name, QString &description) {
    if (m_mode == ClientMode::Local) {
        quint32 board_id = db.insert_board(m_local_id, name, description);
        m_board_menu->create_board(name, description, board_id);
    } else {
        std::string request =
            parser::create_board_request(m_user_id, name, description);
        write(request);
    }
}

void Client::delete_list(int list_index) {
    quint32 list_id = m_current_board->get_list_id(list_index);
    if (m_mode == ClientMode::Local) {
        // db.delete_list(list_id);  // TODO: update when will be correct db api
        db.delete_command("list_signature", "list", list_id);
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
        // db.delete_card(card_id);  // TODO: update when will be correct db api
        db.delete_command("card_signature", "card", card_id);
    } else {
        std::string request = parser::delete_request(card_id, "card");
        write(request);
    }
    m_current_board->delete_card(list_index, card_index);
}

void Client::login(
    QString &username,
    QString &password,
    QString &server_ip,
    QString &server_port
) {
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        if (server_ip == m_server_ip &&
            server_port.toUShort() == m_server_port) {
            return;
        }
        m_socket->disconnectFromHost();
    }
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->waitForDisconnected(-1);
    }
    m_socket->connectToHost(m_server_ip, m_server_port);
    m_server_ip = server_ip;
    m_server_port = server_port.toUShort();
    m_is_authorized = false;
    emit statusChanged();
    if (m_socket->state() == QAbstractSocket::ConnectedState ||
        m_socket->waitForConnected(-1)) {
        std::string login_request = parser::login_request(username, password);
        write(login_request);
    } else {
        qDebug() << "No server!";
        m_connection_status = ConnectionStatus::Unable_to_connect;
        emit connectionStatusChanged();
    }
}