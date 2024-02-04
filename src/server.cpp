#include "server.hpp"
#include <functional>

Server::Server(quint16 port) : port(port) {
    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if (!server->listen(QHostAddress::Any, port)) {
        qDebug() << QObject::tr("Unable to start server: %1").arg(server->errorString());
        return;
    }
}

Server::~Server() {
    for (auto &connection : std::as_const(id_of_all_connections)) {
        connection->close();
    }
}

void Server::newConnection() {
    QTcpSocket *clientSocket = server->nextPendingConnection();
    id_of_all_connections[clientSocket->socketDescriptor()] = clientSocket;
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(removeConnection()));
}

void Server::readyRead() {
    auto clientSocket = dynamic_cast<QTcpSocket*>(sender());
    if (!clientSocket) {
        qDebug() << "Error in casting in signal";
        return;
    }

    auto address = clientSocket->peerAddress();
    
    while (clientSocket->bytesAvailable() > 0) {
        QByteArray data = clientSocket->readAll();

        qDebug() << "Recevied from " << address.toString() << ": " << data;
        clientSocket->write(data);
    }
}

void Server::removeConnection() {
    auto clientSocket = dynamic_cast<QTcpSocket*>(sender());
    if (!clientSocket) {
        qDebug() << "Error in casting in signal";
        return;
    }
    auto client_id = clientSocket->socketDescriptor();
    auto address = clientSocket->peerAddress();

    qDebug() << "Disconnected from " << address.toString();
    clientSocket->close();
    id_of_all_connections.remove(client_id);
}