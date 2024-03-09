#ifndef JSON_PARSER_HPP_
#define JSON_PARSER_HPP_

#include <nlohmann/json.hpp>
#include <QString>
#include <string>

namespace parser {
    using json = nlohmann::json;

    inline std::string get_login_request(QString& username, QString& password) {
        json request = {
            {"request-type", "login"},
            {"user-name", username.toStdString().c_str()},
            {"password", password.toStdString().c_str()}
        }; // Salt here or on server side?
        return request.dump();
    }
} // namespace parser

#endif