#include "client.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include "base_classes.hpp"
#include "boardmenu.hpp"
#include "json_parser.hpp"
#include "logging.hpp"

Client::Client(QObject *parent) : QObject(parent) {
    m_socket = new QTcpSocket(this);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

void Client::create_board() {
    m_current_board = new BoardModel(this);
    emit boardChanged();
}

void Client::write(std::string &data) {
    QByteArray msg = data.c_str();
    QByteArray sz;
    QDataStream sendStream(&sz, QIODevice::WriteOnly);
    quint16 size = msg.size();
    sendStream << size;
    rDebug() << size;
    m_socket->write(sz + msg);
}

void Client::readData() {
    QDataStream in(m_socket);
    if (in.status() == QDataStream::Ok) {
        QDataStream size_data = m_socket->read(2);
        quint16 size;
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
            create_board_select_menu(avaliable_boards);
            m_status = ClientStatus::connected_to_server;
            emit statusChanged();
        } else {
            m_status = ClientStatus::authentification_failed;
            emit statusChanged();
            qDebug() << "Wrong password!";
        }
    }
    if (response_type == "create-response") {
        // validate
        // quint16 board_id = response["board-id"];
        // quint16 list_id = response["list-id"];
        // quint16 card_id = response["card-id"];
        // if (card_id != 0) {
        //     Card new_card = parser::parse_card(response["object"]);
        //     emit create_card(list_id, new_card);
        // }
    }

    if (response_type == "board") {
        // validate
        m_current_board = new BoardModel(this, response);
        loaded_boards.insert(response["id"], m_current_board);
        emit boardChanged();
    }
}

void Client::create_board_select_menu(const nlohmann::json &avaliable_boards) {
    m_board_menu = new BoardMenu(this);  // TODO: Think where to put it properly
    connect(
        m_board_menu, SIGNAL(request_board(int)), this, SLOT(requst_board(int))
    );
    for (auto board : avaliable_boards) {
        QString name = QString::fromStdString(board["name"]);
        QString description = QString::fromStdString(board["description"]);
        quint16 id = board["id"];
        m_board_menu->create_board(name, description, id);
    }
    emit statusChanged();
    emit menuChanged();
}

void Client::request_board(int index) {
    quint16 board_id = m_board_menu->get_id(index);
    // BoardType type = m_board_menu->get_type(index);
    BoardType type = BoardType::remote;
    if (type == BoardType::local) {
        // request from db;
    }
    if (type == BoardType::remote) {
        std::string board_request = parser::board_request(board_id);
        write(board_request);
    }
}

void Client::create_list(QString &name) {
    if (m_status == ClientStatus::connected_to_server) {
        std::string request =
            parser::create_list_request(m_current_board->get_id(), name);
        write(request);
    }
    if (m_status == ClientStatus::local_mode) {
        m_current_board->create_list(name);  // call db, then add with id
    }
}

void Client::create_card(int list_index, QString &name, QString &description) {
    if (m_status == ClientStatus::connected_to_server) {
        std::string request =
            parser::create_card_request(list_index, name, description);
        write(request);
    }
    if (m_status == ClientStatus::local_mode) {
        m_current_board->create_card(
            list_index, name, description
        );  // call db, then add with id
    }
}

void Client::login(
    QString &username,
    QString &password,
    QString &server_ip,
    QString &server_port
) {
    qDebug() << server_ip << server_port << username << password;
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        if (server_ip == m_server_ip &&
            server_port.toUShort() == m_server_port) {
            return;
        }
        m_socket->disconnectFromHost();
    }
    m_server_ip = server_ip;
    m_server_port = server_port.toUShort();
    if (m_socket->state() == QAbstractSocket::UnconnectedState ||
        m_socket->waitForDisconnected(-1)) {
        qDebug("Disconnected!");
    }
    m_socket->connectToHost(m_server_ip, m_server_port);
    if (m_socket->state() == QAbstractSocket::ConnectedState ||
        m_socket->waitForConnected(-1)) {
        std::string login_request = parser::login_request(username, password);
        qDebug() << "Connected!";
        m_status = ClientStatus::waiting_for_response;
        emit statusChanged();
        write(login_request);

        // m_status = ClientStatus::connected_to_server;
        //  nlohmann::json boards = nlohmann::json::array(
        //      {
        //          {
        //              {"name", "board_1"},
        //              {"description", "desc_1"},
        //              {"id", 1}
        //          },
        //          {
        //              {"name", "board_2"},
        //              {"description", "desc_2"},
        //              {"id", 2}
        //          },
        //          {
        //              {"name", "board_3"},
        //              {"description", "desc_3"},
        //              {"id", 3}
        //          },
        //          {
        //              {"name", "board_4"},
        //              {"description", "desc_4"},
        //              {"id", 4}
        //          },
        //      }
        //  );
        //  create_board_select_menu(boards);

    } else {
        qDebug() << "No server!";
        m_status = ClientStatus::unable_to_connect;
        emit statusChanged();
    }
}
