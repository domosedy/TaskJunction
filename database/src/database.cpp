#include "database.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <iostream>
#include <string>
#include "element_classes.hpp"

namespace database {

db_accessor::db_accessor(
    QString database_name,
    QString user_name,
    QString host_name,
    QString password
)
    : m_database_name(std::move(database_name)),
      m_user_name(std::move(user_name)),
      m_host_name(std::move(host_name)),
      m_password(std::move(password)) {
    m_database.setDatabaseName(m_database_name);
    m_database.setUserName(m_user_name);
    m_database.setHostName(m_host_name);
    m_database.setPassword(m_password);
    if (!m_database.open()) {
        qDebug() << "Cannot open database:" << m_database.lastError();
    }
}

void db_accessor::drop_all_tables() {
    QStringList all_tables = m_database.tables();
    QString command = "DROP TABLE %1;";
    foreach (QString str, all_tables) {
        QSqlQuery query;
        query.exec(command.arg(str));
    }
}

void db_accessor::print_all_tables() {
    QStringList all_tables = m_database.tables();
    foreach (QString str, all_tables) {
        qDebug() << "Table:" << str;
    }
}

void db_accessor::clear_all_tables() {
    QStringList all_tables = m_database.tables();
    QString command = "DELETE FROM %1;";
    foreach (QString str, all_tables) {
        QSqlQuery query;
        query.exec(command.arg(str));
    }
}

void db_manager::insert_board(const QString &name) {
    QSqlQuery query;
    query.prepare(query_name_to_sql_command["insert_board"]);
    query.bindValue(":name", name);
    query.exec();
}

void db_manager::insert_list(
    int board_id,
    const QString &name,
    const QString &description
) {
    QSqlQuery query;
    query.prepare(query_name_to_sql_command["insert_list"]);
    query.bindValue(":board_id", board_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.exec();
}

void db_manager::insert_card(
    int list_id,
    const QString &name,
    const QString &description
) {
    QSqlQuery query;
    query.prepare(query_name_to_sql_command["insert_card"]);
    query.bindValue(":list_id", list_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.exec();
}

void db_manager::insert_tag(const QString &name) {
    QSqlQuery query;
    query.prepare(query_name_to_sql_command["insert_tag"]);
    query.bindValue(":name", name);
    query.exec();
}

void db_manager::pin_tag_to_card(int card_id, int tag_id) {
    QSqlQuery query;
    query.prepare(query_name_to_sql_command["insert_into_card_to_tags"]);
    query.bindValue(":card_id", card_id);
    query.bindValue(":tag_id", tag_id);
    query.exec();
}

void db_manager::update_command(
    const QString &table_name,
    const QString &updating_field_type,
    const QString &updating_field_name,
    const QString &key_field_name,
    const QString &new_value,
    unsigned key_value
) {
    QSqlQuery create_function_query;
    create_function_query.exec(
        query_name_to_sql_command["create_update_function"]
            .arg(table_name)
            .arg(updating_field_type)
            .arg(updating_field_name)
            .arg(key_field_name)
    );
    QSqlQuery call_function_query;
    call_function_query.prepare(query_name_to_sql_command["update_function"]
                                    .arg(table_name)
                                    .arg(updating_field_name));
    call_function_query.bindValue(0, new_value);
    call_function_query.bindValue(1, key_value);
    call_function_query.exec();
}

void db_manager::delete_command(
    const QString &table_name,
    const QString &key_field_name,
    unsigned int key_value
) {
    QSqlQuery create_function_query;
    create_function_query.exec(
        query_name_to_sql_command["create_delete_function"]
            .arg(table_name)
            .arg(key_field_name)
    );
    QSqlQuery call_function_query;
    call_function_query.prepare(
        query_name_to_sql_command["delete_function"].arg(table_name)
    );
    call_function_query.bindValue(0, key_value);
    call_function_query.exec();
}

QVector<QVariant> db_manager::get_data(const QSqlRecord &record) {
    QVector<QVariant> result;
    for (int i = 0; i < record.count(); ++i) {
        result.push_back((record.field(i)).value());
    }
    return result;
}

QSqlRecord db_manager::select_info_by_id(
    const QString &table_name,
    const QString &primary_key,
    unsigned key_value
) {
    QSqlQuery query(query_name_to_sql_command["select_info_by_id"]
                        .arg(table_name)
                        .arg(primary_key)
                        .arg(key_value));
    query.next();
    return query.record();
}

board db_manager::select_board(unsigned int id) {
    return board(
        get_data(select_info_by_id(BOARD_TABLE_NAME, BOARD_PRIMARY_KEY, id))
    );
}

list db_manager::select_list(unsigned int id) {
    return std::move(
        list(get_data(select_info_by_id(LIST_TABLE_NAME, LIST_PRIMARY_KEY, id)))
    );
}

card db_manager::select_card(unsigned int id) {
    return std::move(
        card(get_data(select_info_by_id(LIST_TABLE_NAME, LIST_PRIMARY_KEY, id)))
    );
}

tag db_manager::select_tag(unsigned int id) {
    return std::move(
        tag(get_data(select_info_by_id(LIST_TABLE_NAME, LIST_PRIMARY_KEY, id)))
    );
}

}  // namespace database

