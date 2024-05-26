#ifndef JSON_PARSER_HPP_
#define JSON_PARSER_HPP_

#include <QString>
#include <nlohmann/json.hpp>
#include <string>
#include "element_classes.hpp"

namespace parser {
using json = nlohmann::json;

std::string login_request(const QString &username, const QString &password);

std::string create_request(
    const QString &type,
    const quint32 parent_id,
    const QString &name,
    const QString &description,
    const quint32 board_id = 0,
    const quint32 list_id = 0,
    const quint32 card_id = 0,
    const quint32 tag_id = 0
);

std::string board_request(quint32 board_id);

std::string delete_request(
    quint32 id,
    const QString &object_type,
    quint32 board_id,
    quint32 list_id = 0,
    quint32 card_id = 0,
    quint32 tag_id = 0
);

std::string update_request(
    const QString &object_type,
    quint32 id,
    const QString &field,
    const QString &value,
    quint32 board_id,
    quint32 list_id = 0,
    quint32 card_id = 0
);

std::string connect_to_board_request(quint32 group_id, quint32 user_id);

std::string move_request(
    quint32 to_list_id,
    int new_pos,
    quint32 board_id,
    quint32 list_id,
    quint32 card_id
);

tag parse_tag(const json &object);

card parse_card(const json &object, quint32 m_parent_id);

list parse_list(const json &object, quint32 m_parent_id);

board parse_board(const json &object, quint32 m_parent_id);

}  // namespace parser

#endif
