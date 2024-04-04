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

inline std::string create_request(
    const QString &type, 
    const quint32 parent_id,
    const QString &name,
    const QString &description,
    const quint32 board_id = 0, 
    const quint32 list_id = 0,
    const quint32 card_id = 0
) {
    json request = {
        {"type", "create"},
        {"parent-id", parent_id},
        {"board-id", board_id},
        {"list-id", list_id},
        {"card-id", card_id},
        {"name", name.toStdString().c_str()},
        {"description", description.toStdString().c_str()},
        {"object-type", type.toStdString().c_str()}};
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

inline std::string update_request(
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
        {"object-type", object_type.toStdString().c_str()}};
    return request.dump();
}

inline board parse_board(const json &object) {
    QString name = QString::fromStdString(object["name"]);
    QString description = QString::fromStdString(object["description"]);
    quint32 id = object["id"];
    return board(id, 0, name, description);
}

inline list parse_list(const json &object) {
    QString name = QString::fromStdString(object["name"]);
    QString description = QString::fromStdString(object["description"]);
    quint32 id = object["id"];
    return list(id, 0, name, description);
}

inline card parse_card(const json &object) {
    QString name = QString::fromStdString(object["name"]);
    QString description = QString::fromStdString(object["description"]);
    quint32 id = object["id"];
    return card(id, 0, name, description);
}

}  // namespace parser

#endif
