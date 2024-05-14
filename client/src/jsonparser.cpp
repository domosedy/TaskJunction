#include "jsonparser.hpp"
#include <QString>
#include <nlohmann/json.hpp>
#include <string>

namespace parser {
using json = nlohmann::json;

std::string login_request(const QString &username, const QString &password) {
    json request = {
        {"type", "login"},
        {"username", username.toStdString().c_str()},
        {"password", password.toStdString().c_str()}
    };
    return request.dump();
}

std::string create_request(
    const QString &type,
    const quint32 parent_id,
    const QString &name,
    const QString &description,
    const quint32 board_id,
    const quint32 list_id,
    const quint32 card_id
) {
    json request = {
        {"type", "create"},
        {"parent-id", parent_id},
        {"board-id", board_id},
        {"list-id", list_id},
        {"card-id", card_id},
        {"name", name.toStdString().c_str()},
        {"description", description.toStdString().c_str()},
        {"object-type", type.toStdString().c_str()}
    };
    return request.dump();
}

std::string board_request(quint32 board_id) {
    json request = {{"type", "get-boards-info"}, {"id", board_id}};
    return request.dump();
}

std::string delete_request(quint32 id, const QString &object_type) {
    json request = {
        {"type", "delete"},
        {"id", id},
        {"object-type", object_type.toStdString().c_str()}
    };
    return request.dump();
}

std::string update_request(
    const QString &object_type,
    quint32 id,
    const QString &field,
    const QString &value
) {
    json request = {
        {"type", "update"},
        {"id", id},
        {"new-value", value.toStdString().c_str()},
        {"field", field.toStdString().c_str()},
        {"object-type", object_type.toStdString().c_str()}
    };
    return request.dump();
}

std::string move_request(quint32 card_id, quint32 to_list_id, int new_pos) {
    Q_UNUSED(card_id);
    Q_UNUSED(to_list_id);
    Q_UNUSED(new_pos);
    return "";
}

std::string connect_to_board_request(quint32 board_id, quint32 user_id) {
    json request = {
        {"type", "connect"}, {"user_id", user_id}, {"board_id", board_id}
    };
    return request.dump();
}

tag parse_tag(const json &object) {
    QString name = QString::fromStdString(object["name"]);
    quint32 id = object["id"];
    return tag(id, name);
}

card parse_card(const json &object, quint32 m_parent_id) {
    QString name = QString::fromStdString(object["name"]);
    QString description = QString::fromStdString(object["description"]);
    quint32 id = object["id"];
    return card(id, m_parent_id, name, description);
}

list parse_list(const json &object, quint32 m_parent_id) {
    QString name = QString::fromStdString(object["name"]);
    QString description = QString::fromStdString(object["description"]);
    quint32 id = object["id"];
    list list(id, m_parent_id, name, description);
    QVector<card> cards;
    for (const auto &card_json : object["cards"]) {
        card card = parse_card(card_json, id);
        cards.push_back(card);
    }
    list.m_cards = cards;
    return list;
}

board parse_board(const json &object, quint32 m_parent_id) {
    QString name = QString::fromStdString(object["name"]);
    QString description = QString::fromStdString(object["description"]);
    quint32 id = object["id"];
    board board(id, m_parent_id, name, description);
    QVector<list> lists;
    for (const auto &list_json : object["lists"]) {
        list list = parse_list(list_json, id);
        lists.push_back(list);
    }
    board.m_lists = lists;
    return board;
}

}  // namespace parser
