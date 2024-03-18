#include "database.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <iostream>
#include <string>
#include "element_classes.hpp"

namespace database {

db_manager::db_manager(
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

void db_manager::test_foo() {
    QSqlQuery query(m_database);
    //    query.prepare(query_name_to_sql_command["test_multiple_placeholder"]);
    query.prepare("SELECT nspname FROM pg_catalog.pg_namespace;");
    query.exec();
    while (query.next()) {
        auto record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            qDebug() << record.field(i).value();
        }
    }
}

void db_manager::drop_all_tables() {
    QStringList all_tables = m_database.tables();
    QString command = "DROP TABLE %1 CASCADE;";
    foreach (QString str, all_tables) {
        QSqlQuery query(m_database);
        if (!query.exec(command.arg(str))) {
            qDebug() << m_database.lastError();
        }
    }
}

void db_manager::print_all_tables() {
    QStringList all_tables = m_database.tables();
    foreach (QString str, all_tables) {
        qDebug() << "Table:" << str;
    }
}

void db_manager::clear_all_tables() {
    QStringList all_tables = m_database.tables();
    QString command = "DELETE FROM %1;";
    foreach (QString str, all_tables) {
        QSqlQuery query(m_database);
        if (!query.exec(command.arg(str))) {
            qDebug() << m_database.lastError();
        }
    }
}

void db_manager::set_schema(const QString &name) {
    m_schema = name;
}

quint32 db_manager::get_sequence_last_value(const QString &sequence) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["select_last_value"]);
    query.bindValue(":sequence_name", sequence);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

quint32 db_manager::insert_user(const QString &name) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_user"].arg(m_schema));
    query.bindValue(":name", name);
    if (!query.exec()) {
        qDebug() << m_database.lastError()
                 << query_name_to_sql_command["insert_user"].arg(m_schema);
        return 0;
    }
    return get_sequence_last_value(USER_ID_SEQUENCE);
}

quint32 db_manager::insert_board(quint32 user_id, const QString &name) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_board"].arg(m_schema));
    query.bindValue(":user_id", user_id);
    query.bindValue(":name", name);
    if (!query.exec()) {
        qDebug() << "insert_board_error" << m_database.lastError()
                 << query_name_to_sql_command["insert_board"].arg(m_schema);
        return 0;
    }
    return get_sequence_last_value(BOARD_ID_SEQUENCE);
}

quint32 db_manager::insert_list(
    int board_id,
    const QString &name,
    const QString &description
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_list"].arg(m_schema));
    query.bindValue(":board_id", board_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(LIST_ID_SEQUENCE);
}

quint32 db_manager::insert_card(
    int list_id,
    const QString &name,
    const QString &description
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_card"].arg(m_schema));
    query.bindValue(":list_id", list_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(CARD_ID_SEQUENCE);
}

quint32 db_manager::insert_tag(const QString &name) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_tag"].arg(m_schema));
    query.bindValue(":name", name);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(TAG_ID_SEQUENCE);
}

bool db_manager::pin_tag_to_card(int card_id, int tag_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["insert_into_card_to_tags"].arg(m_schema)
    );
    query.bindValue(":card_id", card_id);
    query.bindValue(":tag_id", tag_id);
    if (!query.exec()) {
        qDebug() << m_database.lastError()
                 << query_name_to_sql_command["insert_into_card_to_tags"].arg(
                        m_schema
                    );
        return false;
    }
    return true;
}

bool db_manager::update_command(
    const QString &table_name,
    const QString &updating_field_name,
    const QString &key_field_name,
    const QString &new_value,
    quint32 key_value
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["update_command"].arg(
        m_schema, table_name, updating_field_name, key_field_name
    ));
    query.bindValue(":new_value", new_value);
    query.bindValue(":key_value", key_value);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
        return false;
    }
    return true;
}

bool db_manager::delete_command(
    const QString &table_name,
    const QString &key_field_name,
    quint32 key_value
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["delete_command"].arg(
        m_schema, table_name, key_field_name
    ));
    query.bindValue(":key_value", key_value);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
        return false;
    }
    return true;
}

QVector<QVariant> db_manager::get_data(const QSqlRecord &record) {
    QVector<QVariant> result;
    for (int i = 0; i < record.count(); ++i) {
        result.push_back((record.field(i)).value());
    }
    return result;
}

QSqlRecord
db_manager::select_info_by_id(const QString &query_name, quint32 key_value) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command[query_name].arg(m_schema));
    query.bindValue(":key_value", key_value);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
    }
    query.next();
    return query.record();
}

board db_manager::select_board(quint32 id) {
    return board(get_data(select_info_by_id("select_board", id)));
}

list db_manager::select_list(quint32 id) {
    return list(get_data(select_info_by_id("select_list", id)));
}

card db_manager::select_card(quint32 id) {
    return card(get_data(select_info_by_id("select_card", id)));
}

tag db_manager::select_tag(quint32 id) {
    return tag(get_data(select_info_by_id("select_tag", id)));
}

QVector<list> db_manager::get_board_lists(quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["select_subobject_ids"].arg(
        m_schema, "list_id", "list_signature", "board_id"
    ));
    query.bindValue(":id", board_id);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
    }
    QVector<list> result;
    while (query.next()) {
        result.push_back(select_list(query.value(0).toInt()));
    }
    return result;
}

QVector<card> db_manager::get_list_cards(quint32 list_id) {
    QSqlQuery query(m_database);

    query.prepare(query_name_to_sql_command["select_subobject_ids"].arg(
        m_schema, "card_id", "card_signature", "list_id"
    ));
    query.bindValue(":id", list_id);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
    }
    QVector<card> result;
    while (query.next()) {
        result.push_back(select_card(query.value(0).toInt()));
    }
    return result;
}

QVector<tag> db_manager::get_card_tags(quint32 card_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["select_subobject_ids"].arg(
        m_schema, "tag_id", "card_to_tags", "tag_id"
    ));
    query.bindValue(":id", card_id);
    if (!query.exec()) {
        qDebug() << m_database.lastError();
    }
    QVector<tag> result;
    while (query.next()) {
        result.push_back(select_tag(query.value(0).toInt()));
    }
    return result;
}

bool db_manager::set_user_id(quint32 user_id) {

    m_current_user_id = user_id;
    return true;
}

}  // namespace database

int main(int argc, char *argv[]) {
    if (argc < 5) {
        qDebug() << "wrong arguments format";
        return 1;
    }
    using namespace database;
    db_manager db_manager(argv[1], argv[2], argv[3], argv[4]);
//        db_manager.set_schema("test_schema");
    fill_query_name_to_sql_command();
        db_manager.insert_user("username");
//        db_manager.insert_board(1, "board_name");
//        db_manager.insert_list(1, "list_name", "test1");
//        db_manager.insert_card(1, "card_name", "test");
//        db_manager.insert_tag("tag_name");
//        db_manager.pin_tag_to_card(1, 1);

//        db_manager.update_command(
    //        LIST_TABLE_NAME,  "board_id", LIST_PRIMARY_KEY, "10",2
    //    );
    //    board board = db_manager.select_board(1);
    //    db_manager.delete_command(LIST_TABLE_NAME, LIST_PRIMARY_KEY, 3);
    return 0;
}