#include "data_handler.hpp"
#include <QDebug>
#include <QObject>
#include <optional>
#include <string>
#include <variant>
#include "json.hpp"
#include "logging.hpp"

using json = nlohmann::json;

static std::optional<std::size_t>
get_int_field_data(const json &data, const std::string &field) {
    auto field_it = data.find(field);
    if (field_it == data.end()) {
        return std::nullopt;
    }

    if (!field_it->is_number_unsigned()) {
        return std::nullopt;
    }

    return *field_it;
}

static std::optional<all_ids>
get_object_ids(const json &data) {
    auto board_id = get_int_field_data(data, "board-id");
    auto list_id = get_int_field_data(data, "list-id");
    auto card_id = get_int_field_data(data, "card-id");
    auto tag_id = get_int_field_data(data, "tag-id");

    if (!board_id.has_value() || !list_id.has_value() || !card_id.has_value() || !tag_id.has_value()) {
        return std::nullopt;
    }

    return all_ids{board_id.value(), list_id.value(), card_id.value(), tag_id.value()};
}

static std::optional<std::string>
get_string_field_data(const json &data, const std::string &field) {
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
    auto ids = get_object_ids(json_data);
    auto value_id = get_int_field_data(json_data, "id");
    auto new_value = get_string_field_data(json_data, "new-value");
    auto value_name = get_string_field_data(json_data, "field");
    auto updated_type = get_string_field_data(json_data, "object-type");

    if (!value_id.has_value() || !new_value.has_value() ||
        !value_name.has_value() || !updated_type.has_value() || 
        !ids.has_value()) {
        return std::nullopt;
    }

    return update_query{ids.value(),
        value_id.value(), new_value.value(), value_name.value(),
        updated_type.value()};
}

static std::optional<delete_query> parseDeleteQuery(const json &json_data) {
    auto ids = get_object_ids(json_data);
    auto value_id = get_int_field_data(json_data, "id");
    auto value_type = get_string_field_data(json_data, "object-type");

    if (!value_id.has_value() || !value_type.has_value() ||
        !ids.has_value()) {
        return std::nullopt;
    }

    return delete_query{ids.value(), 
        value_id.value(), value_type.value()};
}

static std::optional<create_query> parseCreateQuery(const json &json_data) {
    auto ids = get_object_ids(json_data);
    auto parent_id = get_int_field_data(json_data, "parent-id");
    auto value_type = get_string_field_data(json_data, "object-type");
    auto value_name = get_string_field_data(json_data, "name");
    auto value_description = get_string_field_data(json_data, "description");

    if (!parent_id.has_value() || !value_type.has_value() ||
        !value_name.has_value() || !value_description.has_value() ||
        !ids.has_value()) {
        return std::nullopt;
    }

    return create_query{ids.value(), 
                        parent_id.value(), value_type.value(), 
                        value_name.value(), value_description.value()};
}

static std::optional<login_query> parseLoginQuery(const json &json_data) {
    auto username = get_string_field_data(json_data, "user-name");
    auto password = get_string_field_data(json_data, "password");

    if (!username.has_value() || !password.has_value()) {
        return std::nullopt;
    }

    return login_query{password.value(), username.value()};
}

static std::optional<get_boards_info_query> parseGetQuery(const json &json_data
) {
    auto id = get_int_field_data(json_data, "id");
    if (!id.has_value()) {
        return std::nullopt;
    }
    return get_boards_info_query{id.value()};
}

static std::optional<move_query> parseMoveQuery(const json &json_data) {
    auto ids = get_object_ids(json_data);
    auto old_list_id = get_int_field_data(json_data, "old-list-id");
    auto new_list_id = get_int_field_data(json_data, "new-list-id");
    auto new_index = get_int_field_data(json_data, "new-index");

    if (!old_list_id.has_value() || !new_list_id.has_value() ||
        !new_index.has_value() || !ids.has_value()) {
        return std::nullopt;
    }

    return move_query{ids.value(), old_list_id.value(), 
                new_list_id.value(), new_index.value()};
}

std::optional<query_type> parseData(const QString &data) {
    if (!json::accept(data.toStdString())) {
        rDebug() << "Bebra";
        return std::nullopt;
    }

    json parsedData = json::parse(data.toStdString());
    auto request_type = get_string_field_data(parsedData, "type");

    if (!request_type.has_value()) {
        return std::nullopt;
    }

    auto request = request_type.value();
    rDebug() << request.c_str();

    std::optional<query_type> result = std::nullopt;
    if (request == "update") {
        result = parseUpdateQuery(parsedData);
    } else if (request == "delete") {
        result = parseDeleteQuery(parsedData);
    } else if (request == "create") {
        result = parseCreateQuery(parsedData);
    } else if (request == "login") {
        result = parseLoginQuery(parsedData);
    } else if (request == "get-boards-info") {
        result = parseGetQuery(parsedData);
    } else if (request == "move") {
        result = parseMoveQuery(parsedData);
    }

    return result;
}