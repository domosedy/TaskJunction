#include "client.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include "json_parser.hpp"

Client::Client(QObject *parent) : QObject(parent) {
    m_socket = new QTcpSocket(this);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

void Client::create_board() {
    m_current_board = new BoardModel(this);
    emit boardChanged();
}

void Client::write(const QString &data) {
    QByteArray msg;
    QDataStream sendStream(&msg, QIODevice::ReadWrite);
    sendStream << quint16(0) << data;

    sendStream.device()->seek(0);
    sendStream << quint16(msg.size() - sizeof(quint16));
    m_socket->write(msg);
}

void Client::readData() {
    QDataStream in(m_socket);
    if (in.status() == QDataStream::Ok) {
        quint16 msg_size = 0;
        while (true) {
            if (!msg_size) {
                if (m_socket->bytesAvailable() <
                    static_cast<qint64>(sizeof(quint16))) {
                    break;
                }
                in >> msg_size;
            }
            qDebug() << msg_size;
            if (m_socket->bytesAvailable() < msg_size) {
                break;
            }
            QString response;
            in >> response;
            qDebug() << response;
            parse_response(response);
            break;
        }
    } else {
        qDebug() << "Error";
    }
}

void Client::parse_response(const QString& data) {
    nlohmann::json response = nlohmann::json::parse(data.toStdString());
    if (!response.contains("response-type")) {
        qDebug() << "Bad response!";
        return;
    }
    std::string response_type = response["response-type"];

    if (response_type == "authorization") {
        if (!response.contains("response") || !response.contains("user-id"))
            qDebug() << "Invalid response structure";
        if (response["response"] == "ok") {
            m_user_id = response["user-id"];
            m_status = ClientStatus::connected_to_server;
            emit statusChanged();
        } else {
            m_status = ClientStatus::authentification_failed;
            emit statusChanged();
            qDebug() << "Wrong password!";
        }
    }
}

void Client::create_list(QString& name) {
    if (m_status == ClientStatus::connected_to_server) {
        std::string request = parser::create_list_request(m_current_board->get_id(), name);
        write(QString::fromStdString(request));
    }
    if (m_status == ClientStatus::local_mode) {
        m_current_board->add_list(name); // call db, then add with id
    }
}

void Client::create_card(int list_index, QString& name, QString& description) {
    if (m_status == ClientStatus::connected_to_server) {
        std::string request = parser::create_card_request(list_index, name, description);
        write(QString::fromStdString(request));
    }
    if (m_status == ClientStatus::local_mode) {
        m_current_board->add_card(list_index, name, description); // call db, then add with id
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
        std::string login_request =
            parser::get_login_request(username, password);
        qDebug() << "Connected!";
        m_status = ClientStatus::waiting_for_response;
        emit statusChanged();
        write(QString::fromStdString(login_request));        
    } else {
        qDebug() << "No server!";
        m_status = ClientStatus::unable_to_connect ;
        emit statusChanged();
    }
}
