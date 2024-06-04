#ifndef QUERY_HPP_
#define QUERY_HPP_

#include <QStringList>
#include <string>
#include <variant>
#include "element_classes.hpp"

enum class FilterType { ANY, ALL };

struct update_query {
    all_ids all_id;
    quint32 value_id;
    std::string new_value;
    std::string value_name;
    std::string updated_type;
};

struct delete_query {
    all_ids all_id;
    quint32 value_id;
    std::string value_type;
};

struct create_query {
    all_ids all_id;
    quint32 parent_id;
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
    quint32 old_list_id;
    quint32 new_list_id;
    quint32 new_index;
};

struct get_boards_info_query {
    quint32 id;
};

struct access_to_board {
    quint32 board_id;
    std::string link;
};

struct copy_board_query {
    board board_to_copy;
};

struct filter_query {
    QStringList tags;
    quint32 board_id;
    FilterType type;
};

using query_type = std::variant<
    update_query,
    delete_query,
    create_query,
    login_query,
    move_query,
    get_boards_info_query,
    access_to_board,
    copy_board_query,
    filter_query>;

#endif