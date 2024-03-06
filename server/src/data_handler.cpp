#include "data_handler.hpp"
#include <QDebug>
#include <QObject>
#include <string>
#include <optional>
#include <optional>
#include <variant>
#include "json.hpp"
#include "logging.hpp"


using json = nlohmann::json;

static std::optional<std::size_t> get_int_field_data(const json &data, const std::string &field) {
    auto field_it = data.find(field);
    if (field_it == data.end()) {
        return std::nullopt;
    }

    if (!field_it->is_number_unsigned()) {
        return std::nullopt;
    }

    return *field_it;
}

static std::optional<std::string> get_string_field_data(const json &data, const std::string &field) {
    auto field_it = data.find(field);
    if (field_it == data.end()) {
        return std::nullopt;
    }

    if (!field_it->is_string()) {
        return std::nullopt;
    }

    return *field_it;
}

static std::optional<update_query> parseUpdateQuery(const json &json_data) {
    auto value_id = get_int_field_data(json_data, "id");
    auto new_value = get_string_field_data(json_data, "new-value");
    auto value_name = get_string_field_data(json_data, "field");
    auto updated_type = get_string_field_data(json_data, "type");

    if (!value_id.has_value() || !new_value.has_value() || !value_name.has_value() || !updated_type.has_value()) {
        return std::nullopt;
    }

    return update_query{
        value_id.value(), new_value.value(), value_name.value(), updated_type.value()
    };
}

static std::optional<delete_query> parseDeleteQuery(const json &json_data) {
    auto value_id = get_int_field_data(json_data, "id");
    auto value_type = get_string_field_data(json_data, "type");

    if (!value_id.has_value() || !value_type.has_value()) {
        return std::nullopt;
    }

    return delete_query{value_id.value(), value_type.value()};
}

static std::optional<create_query> parseCreateQuery(const json &json_data) {
    auto parent_id = get_int_field_data(json_data, "parent-id");
    auto value_type = get_string_field_data(json_data, "type");
    auto value_name = get_string_field_data(json_data, "name");
    
    if (!parent_id.has_value() || !value_type.has_value() || !value_name.has_value()) {
        return std::nullopt;
    }

    return create_query{parent_id.value(), value_type.value(), value_name.value()};
}

static std::optional<login_query> parseLoginQuery(const json &json_data) {
    auto username = get_string_field_data(json_data, "user-name");
    auto password = get_string_field_data(json_data, "password");

    if (!username.has_value() || !password.has_value()) {
        return std::nullopt;
    }

    return login_query{password.value(), username.value()};
}

std::optional<query_type> parseData(const QString &data) {
    json parsedData;
    try {
        parsedData = json::parse(data.toStdString());
    } catch (...) {
        return std::nullopt;
    }

    auto request_type = get_string_field_data(parsedData, "request-type");

    if (!request_type.has_value()) {
        return std::nullopt;
    }

    auto request = request_type.value();
    rDebug() << request;

    std::optional<query_type> result = std::nullopt;
    if (request == "update") {
        result = parseUpdateQuery(parsedData);
    } else if (request == "delete") {
        result = parseDeleteQuery(parsedData);
    } else if (request == "create") {
        result = parseCreateQuery(parsedData);
    } else if (request == "login") {
        result = parseLoginQuery(parsedData);
    }


    return result;
}