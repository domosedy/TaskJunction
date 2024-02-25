#include <QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <iostream>
#include <string>
#include "sql_commands.hpp"

QSqlDatabase db;

static bool create_connection() {
    // TODO
}

void create_board() {
    using namespace sql_query;
    QSqlQuery query;
    query.exec(sql_query::query_name_to_sql_command["create_board_signature"]);
    query.exec(sql_query::query_name_to_sql_command["create_list_signature"]);
    query.exec(sql_query::query_name_to_sql_command["create_card_signature"]);
    query.exec(sql_query::query_name_to_sql_command["create_tag_signature"]);
    query.exec(sql_query::query_name_to_sql_command["create_card_to_tags"]);
}

void print_all_tables() {
    QStringList all_tables = db.tables();
    foreach (QString str, all_tables) {
        qDebug() << "Table:" << str;
    }
}

void drop_table(QString table_name) {
    QSqlQuery query;
    QString command = "DROP TABLE %1";
    query.exec(command.arg(table_name));
}

void drop_all_tables() {
    QStringList all_tables = db.tables();
    foreach (QString str, all_tables) {
        drop_table(str);
    }
}

void insert_board(int board_id, QString name) {
    QSqlQuery query(
        sql_query::query_name_to_sql_command["insert_board"].arg(board_id).arg(
            name
        )
    );
}

void insert_list(int list_id, int board_id, QString name, QString description) {
    QSqlQuery query(sql_query::query_name_to_sql_command["insert_list"]
                        .arg(list_id)
                        .arg(board_id)
                        .arg(name)
                        .arg(description));
}

void insert_card(int card_id, int list_id, QString name, QString description) {
    QSqlQuery query(sql_query::query_name_to_sql_command["insert_card"]
                        .arg(card_id)
                        .arg(list_id)
                        .arg(name)
                        .arg(description));
}

void insert_tag(int tag_id, QString name) {
    QSqlQuery query(
        sql_query::query_name_to_sql_command["insert_tag"].arg(tag_id).arg(name)
    );
}

void pin_tag_to_card(int card_id, int tag_id) {
    QSqlQuery query(sql_query::query_name_to_sql_command["pin_tag_to_card"]
                        .arg(card_id)
                        .arg(tag_id));
}

void update_board_name(QString new_name, int board_id) {
    QSqlQuery query(sql_query::query_name_to_sql_command["update_board_name"]
                        .arg(new_name)
                        .arg(board_id));
}

void update_command(
    QString table_name,
    QString updating_field_name,
    QString new_field_value,
    QString key_field_without_id_postfix,
    int key_value
) {
    QSqlQuery query(sql_query::query_name_to_sql_command["update_command"]
                        .arg(table_name)
                        .arg(updating_field_name)
                        .arg(new_field_value)
                        .arg(key_field_without_id_postfix)
                        .arg(key_value));
}

void select_info_by_id(
    QString table_name,
    QString key_field_without_id_postfix,
    int key_value
) {
    QSqlQuery query(sql_query::query_name_to_sql_command["select_info_by_id"]
                        .arg(table_name)
                        .arg(key_field_without_id_postfix)
                        .arg(key_value));
    query.record();
    // TODO: print_all_fields
}

