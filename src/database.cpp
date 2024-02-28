#include "database.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <iostream>
#include <string>

QSqlDatabase db;

static bool create_connection() {
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("postgres");
    db.setUserName("ivan");
    db.setHostName("localhost");
    db.setPassword("");
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError();
        return false;
    }
    return true;
}

void create_board() {
    using namespace sql_query;
    QSqlQuery query;
    query.exec(sql_query::query_name_to_sql_command["create_board_signature"]);
    query.exec(sql_query::query_name_to_sql_command["create_list_signature"]);
    query.exec(sql_query::query_name_to_sql_command["create_card_signature"]);
    query.exec(sql_query::query_name_to_sql_command["create_tag_signature"]);
    query.exec(sql_query::query_name_to_sql_command["create_card_to_tags"]);
    query.exec(
        sql_query::query_name_to_sql_command["create_insert_board_function"]
    );
    query.exec(
        sql_query::query_name_to_sql_command["create_insert_list_function"]
    );
    query.exec(
        sql_query::query_name_to_sql_command["create_insert_card_function"]
    );
    query.exec(
        sql_query::query_name_to_sql_command["create_insert_tag_function"]
    );
    query.exec(sql_query::query_name_to_sql_command
                   ["create_insert_into_card_to_tags_function"]);
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
    QSqlQuery query;
    query.prepare(sql_query::query_name_to_sql_command["insert_board"]);
    query.bindValue(":board_id", board_id);
    query.bindValue(":name", name);
    query.exec();
}

void insert_list(int list_id, int board_id, QString name, QString description) {
    QSqlQuery query;
    query.prepare(sql_query::query_name_to_sql_command["insert_list"]);
    query.bindValue(":list_id", list_id);
    query.bindValue(":board_id", board_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.exec();
}

void insert_card(int card_id, int list_id, QString name, QString description) {
    QSqlQuery query;
    query.prepare(sql_query::query_name_to_sql_command["insert_card"]);
    query.bindValue(":card_id", card_id);
    query.bindValue(":list_id", list_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.exec();
}

void insert_tag(int tag_id, QString name) {
    QSqlQuery query;
    query.prepare(sql_query::query_name_to_sql_command["insert_tag"]);
    query.bindValue(":tag_id", tag_id);
    query.bindValue(":name", name);
    query.exec();
}

void pin_tag_to_card(int card_id, int tag_id) {
    QSqlQuery query;
    query.prepare(
        sql_query::query_name_to_sql_command["insert_into_card_to_tags"]
    );
    query.bindValue(":card_id", card_id);
    query.bindValue(":tag_id", tag_id);
    query.exec();
}

void update_board_name(QString new_name, int board_id) {
    QSqlQuery query(sql_query::query_name_to_sql_command["update_board_name"]
                        .arg(new_name)
                        .arg(board_id));
}

void update_command(
    QString table_name,
    QString updating_field_type,
    QString key_field_type,
    QString updating_field_name,
    QString key_field_name,
    QString new_value,
    int key_value
) {
    QSqlQuery create_function_query;
    create_function_query.exec(
        sql_query::query_name_to_sql_command["create_update_function"]
            .arg(table_name)
            .arg(updating_field_type)
            .arg(key_field_type)
            .arg(updating_field_name)
            .arg(key_field_name)
    );
    QSqlQuery call_function_query;
    call_function_query.prepare(sql_query::query_name_to_sql_command["update"]
                                    .arg(table_name)
                                    .arg(updating_field_name));
    call_function_query.bindValue(0, new_value);
    call_function_query.bindValue(1, key_value);
    call_function_query.exec();
}

void select_info_by_id(
    QString table_name,
    QString key_field_name,
    int key_value
) {
    QSqlQuery query(sql_query::query_name_to_sql_command["select_info_by_id"]
                        .arg(table_name)
                        .arg(key_field_name)
                        .arg(key_value));
    while (query.next()) {
        QSqlRecord rec = query.record();
        for (int i = 0; i < rec.count(); ++i) {
            QSqlField field = rec.field(i);
            qDebug() << field.name() << ": " << field.value();
        }
    }
}

int main() {
    create_connection();
    sql_query::fill_query_name_to_sql_command();
    create_board();
    print_all_tables();

    insert_board(100, "test new time of query");
    insert_list(100, 100, "test_100", "test new time of query");
    insert_card(100, 100, "test_100", "test new time of query");
    insert_tag(100, "test_100");
    pin_tag_to_card(100, 100);
    update_command(
        "board_signature", "text", "integer", "name", "board_id",
        "name after updating", 0
    );
    select_info_by_id("card_signature", "card_id", 100);
    return 0;
}
