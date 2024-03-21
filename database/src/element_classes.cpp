#include "element_classes.hpp"

user::user(quint32 user_id, QString name) : m_user_id(user_id), m_name(std::move(name)) {
}

board::board(quint32 board_id, quint32 user_id, QString name, QString description)
        : m_board_id(board_id),
          m_user_id(user_id),
          m_name(std::move(name)),
          m_description(std::move(description)) {
}

list::list(quint32 list_id, quint32 board_id, QString name, QString description)
        : m_list_id(list_id),
          m_board_id(board_id),
          m_name(std::move(name)),
          m_description(std::move(description)) {
}

card::card(quint32 card_id, quint32 list_id, QString name, QString description)
        : m_card_id(card_id),
          m_list_id(list_id),
          m_name(std::move(name)),
          m_description(std::move(description)) {
}

tag::tag(quint32 tag_id, QString name) : m_tag_id(tag_id), m_name(std::move(name)) {
}
