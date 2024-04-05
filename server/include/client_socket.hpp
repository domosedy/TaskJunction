#ifndef CLIENT_SOCKET_HPP_
#define CLIENT_SOCKET_HPP_

#include <QByteArray>
#include <QObject>
#include <QTcpSocket>
#include <memory>
#include "query.hpp"

class ClientSocket : public QObject {
    Q_OBJECT

private:
    QTcpSocket *socket;
    uint user_id;

public:
    ClientSocket(QTcpSocket *sock, uint user_id)
        : socket(sock), user_id(user_id) {
        connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(removeConnection()));
    }

    ~ClientSocket() {
        socket->close();
    }

    uint get_client_id() const noexcept {
        return user_id;
    }

    quintptr get_socket_id() const {
        return socket->socketDescriptor();
    }

    void set_client_id(uint new_id) noexcept {
        user_id = new_id;
    }

    ClientSocket(const ClientSocket &) = delete;
    ClientSocket &operator=(const ClientSocket &) = delete;

public slots:
    void sendData(const QByteArray &data);
    void readData();
    void removeConnection();

signals:
    void request_to_database(uint user_id, const query_type &query);
    void disconnected();
};

#endif