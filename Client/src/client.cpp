#include "client.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include "json_parser.hpp"
#include <string>

Client::Client(QObject *parent): QObject(parent) {
    qDebug() << "Created!";
    m_socket = new QTcpSocket(this);

    connect(m_socket, SIGNAL(readyRead()),this,SLOT(readData()));
}

void Client::create_board() {
    qDebug() <<" creating board from client";
    m_current_board = new BoardModel(this);
    emit boardChanged();
}

void Client::write(const QString& data) {
    QByteArray msg;
    QDataStream sendStream(&msg, QIODevice::ReadWrite);
    sendStream << quint16(0) << data;

    sendStream.device()->seek(0);
    sendStream << quint16(msg.size() - sizeof(quint16));
    qDebug() << msg;
    m_socket->write(msg); 
}

void Client::readData() {
    QDataStream in(m_socket);
    if (in.status() == QDataStream::Ok) {
        quint16 msg_size = 0;
        qDebug() << "ready!";
        while(true) {
            if (!msg_size) {
                if (m_socket->bytesAvailable() < static_cast<qint64>(sizeof(quint16))) {
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
            //parse_response(response);
            break; 
        }
    } else qDebug() << "Error";
}

// void Client::parse_response(const QString& data) {
//     nlohmann::json request = nlohmann::parse(data);
    
// }

Client::LoginStatus Client::login(QString& username, QString& password, QString& server_ip, QString& server_port) {
    qDebug() << server_ip << server_port << username << password;
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        if (server_ip == m_server_ip && server_port.toUShort() == m_server_port)
            return LoginStatus::same_server;
        m_socket->disconnectFromHost();
    }
    m_server_ip = server_ip;
    m_server_port = server_port.toUShort();
    if (m_socket->state() == QAbstractSocket::UnconnectedState
        || m_socket->waitForDisconnected(-1)) {
            qDebug("Disconnected!");
    }    
    m_socket->connectToHost(m_server_ip, m_server_port);
    if (m_socket->state() == QAbstractSocket::ConnectedState || m_socket->waitForConnected(-1)) {
        std::string login_request = parser::get_login_request(username, password);
        qDebug() << "Connected!";
        write(QString::fromStdString(login_request));
        return LoginStatus::success;
    } else {
        return LoginStatus::invalid_server;
    }
}
