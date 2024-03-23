#include "element_classes.hpp"
#include <sstream>

user::user(quint32 user_id, QString name) : m_user_id(user_id), m_name(std::move(name)) {
}

board::board(quint32 board_id, quint32 user_id, QString name, QString description)
    : m_board_id(board_id),
      m_user_id(user_id),
      m_name(std::move(name)),
      m_description(description) {
}

list::list(quint32 list_id, quint32 board_id, QString name, QString description)
        : m_list_id(list_id),
          m_board_id(board_id),
          m_name(std::move(name)),
          m_description(description) {
}

card::card(quint32 card_id, quint32 list_id, QString name, QString description)
        : m_card_id(card_id),
          m_list_id(list_id),
          m_name(std::move(name)),
          m_description(description) {
}

tag::tag(quint32 tag_id, QString name) : m_tag_id(tag_id), m_name(std::move(name)) {
}

template<typename T> 
requires has_to_json_method<T>
static std::string array_to_json(const QVector<T> &data) {
    std::stringstream ss;

    for (qsizetype i = 0; i + 1 < data.size(); ++i) {
        ss << data[i].to_json() << ", ";
    }

    if (!data.empty()) {
        ss << data.back().to_json();
    }

    return ss.str();
}

std::string tag::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"tag\", \"id\": " << m_tag_id
         << ", \"name\": \"" << m_name.toStdString() << "\" }";
    return ss.str();
}

std::string card::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"card\", \"tags\": [";
    ss << array_to_json(m_tags);

    ss << "], \"id\": " << m_card_id
       << ", \"name\": \"" << m_name.toStdString() << "\"" 
       << ", \"description\": \"" << m_description.toStdString() << "\"}";

    return ss.str();
}

std::string list::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"list\", \"cards\": [";
    ss << array_to_json(m_cards);

    ss << "], \"id\": " << m_list_id
       << ", \"name\": \"" << m_name.toStdString() << "\"" 
       << ", \"description\": \"" << m_description.toStdString() << "\"}";

    return ss.str();
}

std::string board::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"board\", \"lists\": [";
    ss << array_to_json(m_lists);

    ss << "], \"id\": " << m_board_id
       << ", \"name\": \"" << m_name.toStdString() << "\"" 
       << "}";

    return ss.str();
}


std::string error::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"error\", \"msg\": \"" << error_message << "\" }";

    return ss.str();
}

std::string login::to_json() const {
    std::stringstream ss;

    ss << "{\"type\": \"authorization\", \"response\": \"";
    if (m_response) {
        ss << "ok";
    } else {
        ss << "wrong";
    }

    ss << "\", \"boards\": [";

    ss << array_to_json(m_boards);

    ss << "]}";
    return ss.str();
}