#ifndef DATABASE_HPP_
#define DATABASE_HPP_
#include <QtCore>

namespace sql_query {
QMap<QString, QString> query_name_to_sql_command;

void fill_query_name_to_sql_command() {
    query_name_to_sql_command["create_board_signature"] =
        "CREATE TABLE board_signature("
        "board_id integer PRIMARY KEY, "
        "name text"
        ");     ";

    query_name_to_sql_command["create_list_signature"] =
        "CREATE TABLE list_signature("
        "list_id integer PRIMARY KEY,"
        "board_id integer REFERENCES board_signature ON DELETE CASCADE"
        "name text,"
        "description text"
        "); ";

    query_name_to_sql_command["create_card_signature"] =
        "CREATE TABLE card_signature("
        "card_id integer PRIMARY KEY,"
        "list_id integer REFERENCES list_signature ON DELETE CASCADE,"
        "name text,"
        "description text"
        "); ";

    query_name_to_sql_command["create_tag_signature"] =
        "CREATE TABLE tag_signature("
        "tag_id integer PRIMARY KEY,"
        "name text"
        ");";

    query_name_to_sql_command["create_card_to_tags"] =
        "CREATE TABLE card_to_tags("
        "card_id integer REFERENCES card_signature ON DELETE CASCADE, "
        "tag_id integer REFERENCES tag_signature ON DELETE CASCADE, "
        "PRIMARY KEY(card_id, tag_id)"
        ");";

    query_name_to_sql_command["insert_board"] =
        "INSERT INTO board_signature VALUES (%1, '%2');";
    query_name_to_sql_command["insert_list"] =
        "INSERT INTO list_signature VALUES (%1, %2, '%3', '%4');";
    query_name_to_sql_command["insert_card"] =
        "INSERT INTO card_signature VALUES (%1, %2, '%3', '%4');";
    query_name_to_sql_command["insert_tag"] =
        "INSERT INTO tag_signature VALUES (%1, '%2');";
    query_name_to_sql_command["pin_tag_to_card"] =
        "INSERT INTO card_to_tags VALUES (%1, %2);";
    query_name_to_sql_command["update_command"] =
        "UPDATE %1 SET %2 = '%3' WHERE %4_id = %5;";
    query_name_to_sql_command["select_info_by_id"] = 
        "SELECT * FROM %1 WHERE %2 = %3";
}
}  // namespace sql_query

#endif  // DATABASE_HPP_
