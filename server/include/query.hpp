#ifndef QUERY_HPP_
#define QUERY_HPP_

#include <string>
#include <variant>
#include "element_classes.hpp"
// struct a {
    // std::size_t board_id;
    // std::size_t list_id;
    // std::size_t card_id;
    // std::size_t tag_id;
// };

struct update_query {
    all_ids all_id;
    std::size_t value_id;
    std::string new_value;
    std::string value_name;
    std::string updated_type;
};

struct delete_query {
    all_ids all_id;
    std::size_t value_id;
    std::string value_type;
};

struct create_query {
    all_ids all_id;
    std::size_t parent_id;
    std::string value_type;
    std::string value_name;
    std::string value_description;
};

struct login_query {
    std::string password;  // TODO: hash or salt it
    std::string user_name;
};

struct move_query {
    all_ids all_id;
    std::size_t old_list_id;
    std::size_t new_list_id;
    std::size_t new_index;
};

struct get_boards_info_query {
    std::size_t id;
};

using query_type = std::variant<
    update_query,
    delete_query,
    create_query,
    login_query,
    move_query,
    get_boards_info_query>;

#endif