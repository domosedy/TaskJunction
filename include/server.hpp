#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <QTcpServer>
#include <QObject>
#include <QMap>
#include <QTcpSocket>

class Server : public QObject {
Q_OBJECT

    QTcpServer *server;
    quint16 port;
    QMap<quintptr, QTcpSocket*> id_of_all_connections;

public:
    explicit Server(quint16);
    ~Server();

public slots:
    void newConnection();
    void removeConnection();
    void readyRead();
};

#endif