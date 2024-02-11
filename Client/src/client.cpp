#include "client.hpp"

Client::Client(QObject *parent): QObject(parent) {
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost,8080);   
}