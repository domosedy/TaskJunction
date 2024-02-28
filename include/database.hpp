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

        query_name_to_sql_command["create_insert_board_function"] =
                "CREATE FUNCTION insert_board(board_id_ integer, name_ text) RETURNS "
                "void AS $$"
                "INSERT INTO board_signature VALUES (board_id_, name_);"
                "$$ LANGUAGE SQL;";
        query_name_to_sql_command["insert_board"] =
                "SELECT insert_board(:board_id, :name)";

        query_name_to_sql_command["create_insert_list_function"] =
                "CREATE FUNCTION insert_list(list_id_ integer, board_id_ integer, "
                "name_ text, description_ text) RETURNS void AS $$"
                "INSERT INTO list_signature VALUES (list_id_, board_id_, name_, "
                "description_);"
                "$$ LANGUAGE SQL;";
        query_name_to_sql_command["insert_list"] =
                "SELECT insert_list(:list_id, :board_id, :name, :description)";

        query_name_to_sql_command["create_insert_card_function"] =
                "CREATE FUNCTION insert_card(card_id_ integer, list_id_ integer, name_ "
                "text, description_ text) RETURNS void AS $$"
                "INSERT INTO card_signature VALUES (card_id_, list_id_, name_, "
                "description_);"
                "$$ LANGUAGE SQL;";
        query_name_to_sql_command["insert_card"] =
                "SELECT insert_card(:card_id, :list_id, :name, :description)";

        query_name_to_sql_command["create_insert_tag_function"] =
                "CREATE FUNCTION insert_tag(tag_id_ integer, name_ text) RETURNS void "
                "AS $$"
                "INSERT INTO tag_signature VALUES (tag_id_, name_);"
                "$$ LANGUAGE SQL;";
        query_name_to_sql_command["insert_tag"] =
                "SELECT insert_tag(:tag_id, :name)";

        query_name_to_sql_command["create_insert_into_card_to_tags_function"] =
                "CREATE FUNCTION insert_into_card_to_tags(card_id_ integer, tag_id_ "
                "integer) RETURNS void AS $$"
                "INSERT INTO card_to_tags VALUES (card_id_, tag_id_);"
                "$$ LANGUAGE SQL;";
        query_name_to_sql_command["insert_into_card_to_tags"] =
                "SELECT insert_into_card_to_tags(:card_id, :tag_id)";

        query_name_to_sql_command["create_update_function"] =
                "CREATE FUNCTION update_%1_%4(%2, %3) RETURNS void AS $$"
                "UPDATE %1 SET %4 = $1 WHERE %5 = $2;"
                "$$ LANGUAGE SQL;";
        query_name_to_sql_command["update"] = "SELECT update_%1_%4(?, ?)";

        query_name_to_sql_command["select_info_by_id"] =
                "SELECT * FROM %1 WHERE %2 = %3;";
    }
}  // namespace sql_query

#endif  // DATABASE_HPP_
