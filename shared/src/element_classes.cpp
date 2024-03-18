#include "element_classes.hpp"
#include <sstream>

board::board(QVector<QVariant> data)
        : m_board_id(data[0].toInt()), m_name(data[1].toString()) {
}

list::list(QVector<QVariant> data)
        : m_list_id(data[0].toInt()),
          m_board_id(data[1].toInt()),
          m_name(data[2].toString()),
          m_description(data[3].toString()) {
}

card::card(QVector<QVariant> data)
        : m_card_id(data[0].toInt()),
          m_list_id(data[1].toInt()),
          m_name(data[2].toString()),
          m_description(data[3].toString()) {
}

tag::tag(QVector<QVariant> data)
        : m_tag_id(data[0].toInt()), m_name(data[1].toString()) {
}

template<typename T> 
requires has_to_json_method<T>
static std::string array_to_json(const QVector<T> &data) {
    std::stringstream ss;

    for (std::size_t i = 0; i + 1 < data.size(); ++i) {
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


// std::string ErrorJson::to_json() const {
//     std::stringstream ss;
//     ss << "{ \"type\": \"error\", \"msg\": \"" << error_message << "\" }";

//     return ss.str();
// }

std::string login::to_json() const {
    std::stringstream ss;

    ss << "{\"type\": \"authorization\", \"response\": \"";
    if (m_response) {
        ss << "ok";
    } else {
        ss << "wrong";
    }

    ss << "\", \"boards\": [";

    for (std::size_t i = 0; i + 1 < m_boards.size(); ++i) {
        ss << "{ \"name\": \"" << m_boards[i].second.toStdString()
            << "\", \"id\": " << m_boards[i].first << "}, "; 
    }

    if (!m_boards.empty()) {
        ss << "{ \"name\": \"" << m_boards.back().second .toStdString()
            << "\", \"id\": " << m_boards.back().first << "}"; 
    }

    ss << "]}";
    return ss.str();
}