#include "server.hpp"
#include <functional>
#include <memory>
#include "client_socket.hpp"
#include "logging.hpp"
#include "query.hpp"

// How to emit when my connection is not authorized?
Server::Server(quint16 port) : port(port) {
    server = new QTcpServer(this);
    db = new Database();

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(
        db, SIGNAL(executed_query(int, const QString &)), this,
        SLOT(database_executed(int, const QString &))
    );

    if (!server->listen(QHostAddress::Any, port)) {
        rDebug() << QObject::tr("Unable to start server: %1")
                        .arg(server->errorString());
        std::exit(1);
    }
}

Server::~Server() {
}

void Server::newConnection() {
    static int id = 0;

    QTcpSocket *clientSocket = server->nextPendingConnection();
    ClientSocket *clientSock = new ClientSocket(clientSocket, id);
    auto socket_id = clientSock->get_socket_id();
    id_of_all_connections[socket_id] = clientSock;
    pointer_to_authorized[id] = socket_id;

    connect(
        id_of_all_connections[socket_id],
        SIGNAL(request_to_database(int, std::shared_ptr<query>)), db,
        SLOT(execute_query(int, std::shared_ptr<query>))
    );

    ++id;

    connect(
        id_of_all_connections[socket_id], SIGNAL(disconnected()), this,
        SLOT(removeConnection())
    );
}

void Server::readyRead() {
    auto clientSocket = dynamic_cast<QTcpSocket *>(sender());
    if (!clientSocket) {
        rDebug() << "Error in casting in signal";
        return;
    }

    auto address = clientSocket->peerAddress();
    QByteArray data = clientSocket->readAll();
    rDebug() << "Recevied from " << address.toString() << ": " << data;

    clientSocket->write(data);
}

void Server::database_executed(int user_id, const QString &query) {
    id_of_all_connections[pointer_to_authorized[user_id]]->sendData(
        query.toUtf8()
    );
}

void Server::removeConnection() {
    auto clientSocket = dynamic_cast<ClientSocket *>(sender());
    if (!clientSocket) {
        rDebug() << "Error in casting in signal";
        return;
    }

    pointer_to_authorized.remove(clientSocket->get_client_id());
    id_of_all_connections.remove(clientSocket->get_socket_id());
    delete clientSocket;
}