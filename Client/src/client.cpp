#include "client.hpp"

Client::Client(QObject *parent): QObject(parent) {

}

void Client::create_board() {
    m_current_board = new BoardModel(this);
    emit boardChanged();
}

BoardModel* Client::get_current_board() {
    return m_current_board;
}
