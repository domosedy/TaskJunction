#ifndef JSON_PARSER_HPP_
#define JSON_PARSER_HPP_

#include <QString>
#include <nlohmann/json.hpp>
#include <string>

namespace parser {
using json = nlohmann::json;

inline std::string
login_request(const QString &username, const QString &password) {
    json request = {
        {"type", "login"},
        {"user-name", username.toStdString().c_str()},
        {"password",
         password.toStdString().c_str()}};  // Salt here or on server side?
    return request.dump();
}

inline std::string create_list_request(quint32 board_id, const QString &name) {
    json request = {
        {"type", "create"},
        {"parent-id", board_id},
        {"name", name.toStdString().c_str()},
        {"type", "list"}};
    return request.dump();
}

inline std::string create_card_request(
    quint32 list_id,
    const QString &name,
    const QString &description
) {
    json request = {
        {"type", "create-card"},
        {"parent-id", list_id},
        {"name", name.toStdString().c_str()},
        {"description", description.toStdString().c_str()},
        {"type", "list"}};
    return request.dump();
}

inline std::string create_board_request(
    quint32 user_id,
    const QString &name,
    const QString &description
) {
    json request = {
        {"type", "create-board"},
        {"parent-id", user_id},
        {"name", name.toStdString().c_str()},
        {"description", description.toStdString().c_str()},
        {"type", "board"}};
    return request.dump();
}

inline std::string board_request(quint32 board_id) {
    json request = {{"type", "get-board-info"}, {"id", board_id}};
    return request.dump();
}

inline std::string delete_request(quint32 id, const QString &object_type) {
    json request = {
        {"type", "delete"},
        {"id", id},
        {"object-type", object_type.toStdString().c_str()}};
    return request.dump();
}

inline board parse_board(const json &object) {
    QString name = QString::fromStdString(object["name"]);
    QString description = QString::fromStdString(object["description"]);
    quint32 id = object["id"];
    return board(id, 0, name, description);
}

// inline Card parse_card(const json& card) {
//     return Card();
// }
}  // namespace parser

#endif
