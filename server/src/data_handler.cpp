#include "data_handler.hpp"
#include <QDebug>
#include <QObject>
#include <exception>
#include <string>
#include "json.hpp"
#include "logging.hpp"

using json = nlohmann::json;

static std::shared_ptr<query> parseUpdateQuery(const json &json_data) {
    std::size_t value_id = json_data.at("id");
    std::string new_value = json_data.at("value");
    std::string value_name = json_data.at("value-name");
    std::string updated_type = json_data.at("type");

    return std::make_shared<update_query>(
        value_id, new_value, value_name, updated_type
    );
}

static std::shared_ptr<query> parseDeleteQuery(const json &json_data) {
    std::size_t value_id = json_data.at("id");
    std::string value_type = json_data.at("type");

    return std::make_shared<delete_query>(value_id, value_type);
}

static std::shared_ptr<query> parseCreateQuery(const json &json_data) {
    std::size_t parent_id = json_data.at("parent-id");
    std::string value_type = json_data.at("type");
    std::string value_name = json_data.at("name");

    return std::make_shared<create_query>(parent_id, value_type, value_name);
}

std::shared_ptr<query> parseData(const QString &data) {
    try {
        json parsedData = json::parse(data.toStdString());

        std::string request_type = parsedData.at("request-type");

        if (request_type == "update") {
            return parseUpdateQuery(parsedData);
        } else if (request_type == "delete") {
            return parseDeleteQuery(parsedData);
        } else if (request_type == "create") {
            return parseCreateQuery(parsedData);
        }

        throw std::runtime_error("Unknown request type");
    } catch (std::exception &e) {
        rDebug() << e.what();
        return nullptr;
    }
}