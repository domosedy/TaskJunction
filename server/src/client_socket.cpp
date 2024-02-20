#include "client_socket.hpp"
#include <QDataStream>
#include <sstream>
#include "data_handler.hpp"
#include "logging.hpp"

void ClientSocket::sendData(const QByteArray &data) {
    QByteArray data_size;
    QDataStream out(&data_size, QIODevice::WriteOnly);
    out << data.size();

    socket->write(data_size);
    socket->write(data);
}

void ClientSocket::readData() {
    try {
        QByteArray data = socket->readLine();
        auto data_string = data.toStdString();

        std::stringstream in(data_string);
        in.exceptions(std::ios::failbit | std::ios::badbit);
        int size;
        in >> size;

        data = socket->read(size);
        QString json_request = data.toStdString().c_str();

        rDebug() << "Received from " << socket->peerAddress().toString() << size
                 << " " << json_request;

        auto value = parseData(json_request);

        if (value == nullptr) {
            throw std::runtime_error("nullptr recevied");
        }

        emit request_to_database(user_id, std::move(value));
    } catch (std::exception &e) {
        rDebug() << e.what();
        sendData("Bad format of data");
    }
}

void ClientSocket::removeConnection() {
    emit disconnected();
}
