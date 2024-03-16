#ifndef JSON_PARSER_HPP_
#define JSON_PARSER_HPP_

#include <QString>
#include <nlohmann/json.hpp>
#include <string>

namespace parser {
using json = nlohmann::json;

inline std::string login_request(QString &username, QString &password) {
    json request = {
        {"request-type", "login"},
        {"user-name", username.toStdString().c_str()},
        {"password",
         password.toStdString().c_str()}};  // Salt here or on server side?
    return request.dump();
}

inline std::string create_list_request(quint16 board_id, QString &name) {
    json request = {
        {"type", "create"},
        {"parent-id", board_id},
        {"name", name.toStdString().c_str()},
        {"type", "list"}};
    return request.dump();
}

inline std::string
create_card_request(quint16 list_id, QString &name, QString &description) {
    json request = {
        {"type", "create-card"},
        {"parent-id", list_id},
        {"name", name.toStdString().c_str()},
        {"name", description.toStdString().c_str()},
        {"type", "list"}};
    return request.dump();
}

inline std::string board_request(quint16 board_id) {
    json request = {{"type", "get-board-info"}, {"id", board_id}};
    return request.dump();
}

// inline Card parse_card(const json& card) {
//     return Card();
// }
}  // namespace parser

#endif