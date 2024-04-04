#include "client_socket.hpp"
#include <QDataStream>
#include <sstream>
#include "data_handler.hpp"
#include "logging.hpp"
#include "datas.hpp"

void ClientSocket::sendData(const QByteArray &data) {
    rDebug() << data.size();

    quint16 size = data.size();
    QByteArray data_size;
    QDataStream out(&data_size, QIODevice::WriteOnly);
    out << size;

    socket->write(data_size + data);
}

void ClientSocket::readData() {
    QDataStream size_data = socket->read(2);
    quint16 size;

    size_data >> size;
    rDebug() << "readed size " << size;

    QByteArray data = socket->read(size);
    
    QString json_request = data.toStdString().c_str();

    rDebug() << "Received from " << socket->peerAddress().toString() 
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
