#include "client_socket.hpp"
#include <QDataStream>
#include <sstream>
#include "data_handler.hpp"
#include "element_classes.hpp"
#include "logging.hpp"

void ClientSocket::sendData(const QString &data) {
    rDebug() << data;
    socket->sendTextMessage(data);
}

void ClientSocket::readData(const QString &data) {
    rDebug() << "Received from " << socket->peerAddress().toString() << " "
             << data;

    auto value = parseData(data);

    if (!value.has_value()) {
        sendData(error{"Bad format of data"}.to_json().c_str());
        return;
    }

    emit request_to_database(user_id, std::move(value.value()));
}

void ClientSocket::removeConnection() {
    emit disconnected();
}
