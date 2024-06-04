#include "element_classes.hpp"
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>

user::user(quint32 user_id, QString name)
    : m_user_id(user_id), m_name(std::move(name)) {
}

board::board(
    quint32 board_id,
    quint32 user_id,
    QString name,
    QString description,
    QString link,
    bool is_remote
)
    : m_board_id(board_id),
      m_user_id(user_id),
      m_name(std::move(name)),
      m_description(std::move(description)),
      m_link(std::move(link)),
      m_is_remote(is_remote) {
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

tag::tag(quint32 tag_id, QString name)
    : m_tag_id(tag_id), m_name(std::move(name)) {
}

template <typename T>
    requires has_to_json_method<T>
static std::string array_to_json(const QVector<T> &data) {
    std::stringstream ss;

    for (qsizetype i = 0; i + 1 < data.size(); ++i) {
        ss << data[i].to_json() << ",";
    }

    if (!data.empty()) {
        ss << data.back().to_json();
    }

    return ss.str();
}

std::string all_ids::to_json() const {
    std::stringstream ss;
    ss << R"("board-id":)" << board_id << R"(,"list-id":)" << list_id
       << R"(,"card-id":)" << card_id << R"(,"tag-id":)" << tag_id;

    return ss.str();
}

std::string tag::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\":\"tag\",\"id\":" << m_tag_id << ",\"name\":\""
       << m_name.toStdString() << "\"}";
    return ss.str();
}

std::string card::to_json() const {
    std::stringstream ss;
    ss << "{\"type\":\"card\",\"tags\":[";
    ss << array_to_json(m_tags);

    ss << "],\"id\":" << m_card_id << ",\"name\":\"" << m_name.toStdString()
       << "\""
       << ",\"description\":\"" << m_description.toStdString() << "\"}";

    return ss.str();
}

std::string list::to_json() const {
    std::stringstream ss;
    ss << "{\"type\":\"list\",\"cards\":[";
    ss << array_to_json(m_cards);

    ss << "],\"id\":" << m_list_id << ",\"name\":\"" << m_name.toStdString()
       << "\""
       << ",\"description\":\"" << m_description.toStdString() << "\"}";

    return ss.str();
}

std::string board::to_json() const {
    std::stringstream ss;
    ss << "{\"type\":\"board\",\"lists\":[";
    ss << array_to_json(m_lists);

    ss << "],\"id\":" << m_board_id << ",\"name\":\"" << m_name.toStdString()
       << "\""
       << ",\"description\":\"" << m_description.toStdString()
       << "\",\"link\":\"" << m_link.toStdString() << "\"}";

    return ss.str();
}

std::string error::to_json() const {
    std::stringstream ss;
    ss << "{\"type\":\"error\",\"msg\":\"" << error_message << "\"}";

    return ss.str();
}

std::string login::to_json() const {
    std::stringstream ss;

    ss << "{\"type\":\"authorization\",\"response\":\"";
    if (m_response) {
        ss << "ok";
    } else {
        ss << "wrong";
    }

    ss << "\",\"boards\":[";

    ss << array_to_json(m_boards);

    ss << "]}";
    return ss.str();
}

std::string create_response::to_json() const {
    std::stringstream ss;

    ss << "{\"type\":\"create\",";
    ss << ids.to_json();
    ss << ",\"object-json\":" << jsoned_object << "}";

    return ss.str();
}

std::string update_response::to_json() const {
    std::stringstream ss;
    ss << R"({"type":"update",)";
    ss << ids.to_json() << ",";

    ss << R"("new-value":")" << new_value.toStdString() << R"(","field":")"
       << field.toStdString() << R"("})";
    return ss.str();
}

std::string delete_response::to_json() const {
    std::stringstream ss;
    ss << R"({"type":"delete",)";
    ss << ids.to_json() << "}";
    return ss.str();
}

std::string move_response::to_json() const {
    std::stringstream ss;
    ss << R"({"type":"move",)";
    ss << ids.to_json() << R"(,"old-list-id":)" << old_list_id
       << R"(,"new-list-id":)" << new_list_id << R"(,"new-index":)" << new_index
       << "}";
    return ss.str();
}

std::string filter_response::to_json() const {
    nlohmann::json data = {{"type", "filter"}, {"board-id", board_id}, {"tags", tag_ids}};

    return data.dump();
}