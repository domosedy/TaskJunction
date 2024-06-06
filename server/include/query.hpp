#ifndef QUERY_HPP_
#define QUERY_HPP_

#include <string>
#include <variant>

struct update_query {
    std::size_t value_id;
    std::string new_value;
    std::string value_name;
    std::string updated_type;
};

struct delete_query {
    std::size_t value_id;
    std::string value_type;
};

struct create_query {
    std::size_t parent_id;
    std::size_t board_id;
    std::size_t list_id;
    std::size_t card_id;
    std::string value_type;
    std::string value_name;
    std::string value_description;
};

struct login_query {
    std::string password;  // TODO: hash or salt it
    std::string user_name;
};

struct get_boards_info_query {
    std::size_t id;
};

using query_type = std::variant<
    update_query,
    delete_query,
    create_query,
    login_query,
    get_boards_info_query>;

#endif
