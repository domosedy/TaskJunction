#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QObject>
#include <QTcpSocket>
#include "boardmodel.hpp"

class Client : public QObject {
    Q_OBJECT
    Q_PROPERTY(BoardModel* current_board MEMBER m_current_board READ get_current_board NOTIFY boardChanged)

public:
    explicit Client(QObject *parent = nullptr);
    Q_INVOKABLE void create_board();
    BoardModel* get_current_board();
signals:
    void boardChanged();

private:
    QTcpSocket *socket = nullptr;
    BoardModel* m_current_board;
};

#endif  // CLIENT_HPP
