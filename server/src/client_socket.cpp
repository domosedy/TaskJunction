#include "client_socket.hpp"
#include <QDataStream>
#include <sstream>
#include "data_handler.hpp"
#include "logging.hpp"
#include "datas.hpp"

void ClientSocket::sendData(const QByteArray &data) {
    rDebug() << data.size();
    QByteArray data_size;
    QDataStream out(&data_size, QIODevice::WriteOnly);
    out << QVariant(data.size()).toString() << "\n";

    socket->write(data_size + data);
}

void ClientSocket::readData() {
    QByteArray data = socket->readLine();
    auto data_string = data.toStdString();

    std::stringstream ss(data_string);
    std::size_t size;

    if (!(ss >> size)) {
        sendData(ErrorJson{"Bad format of data"}.to_json().c_str());
        return;
    }

    data = socket->read(size);
    QString json_request = data.toStdString().c_str();

    rDebug() << "Received from " << socket->peerAddress().toString() << size
                << " " << json_request;

    auto value = parseData(json_request);

    if (!value.has_value()) {
        sendData(ErrorJson{"Bad format of data"}.to_json().c_str());
        return;
    }

    emit request_to_database(user_id, std::move(value.value()));
}

void ClientSocket::removeConnection() {
    emit disconnected();
}
