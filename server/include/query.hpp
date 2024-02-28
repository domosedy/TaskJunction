#ifndef QUERY_HPP_
#define QUERY_HPP_

#include <string>
#include <variant>

struct update_query {
    std::size_t value_id;
    std::string new_value;
    std::string value_name;
    std::string updated_type;

    update_query(
        std::size_t value_id,
        std::string &new_value,
        std::string &value_name,
        std::string &updated_type
    )
        : value_id(value_id),
          new_value(std::move(new_value)),
          value_name(std::move(value_name)),
          updated_type(std::move(updated_type)) {
    }

};

struct delete_query {
    std::size_t value_id;
    std::string value_type;

    delete_query(std::size_t value_id, std::string &value_type)
        : value_id(value_id), value_type(std::move(value_type)) {
    }
};

struct create_query {
    std::size_t parent_id;
    std::string value_type;
    std::string value_name;

    create_query(
        std::size_t parent_id,
        std::string &value_type,
        std::string &value_name
    )
        : parent_id(parent_id),
          value_type(std::move(value_type)),
          value_name(std::move(value_name)) {
    }
};

struct login_query {
    std::string password; // TODO: hash or salt it
    std::string user_name;

    login_query(
        std::string &password,
        std::string &user_name
    ) : password(std::move(password)), user_name(std::move(user_name)) {
    }
};

using query_type = std::variant<update_query, delete_query, create_query, login_query>;


#endif