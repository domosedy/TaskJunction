#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QObject>
#include <QTcpSocket>
#include "board.hpp"

class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    void create_board();

private:
    QTcpSocket *socket = nullptr;
    //Board* board = nullptr;
};

#endif  // CLIENT_HPP
