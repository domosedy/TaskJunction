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
    fill_query_name_to_sql_command();
}

QMap<QString, QString> db_manager::query_name_to_sql_command;

void db_manager::fill_query_name_to_sql_command() {
    query_name_to_sql_command["insert_user_data"] =
        "INSERT INTO %1.user_authorization_data VALUES (:login, :password);";

    query_name_to_sql_command["insert_user_signature"] =
        "INSERT INTO %1.user_signature VALUES (DEFAULT, :login);";

    query_name_to_sql_command["insert_board"] =
        "INSERT INTO %1.board_signature VALUES (DEFAULT, :user_id, :name, "
        ":description);";

    query_name_to_sql_command["insert_list"] =
        "INSERT INTO %1.list_signature VALUES (DEFAULT, :board_id, :name, "
        ":description);";

    query_name_to_sql_command["insert_card"] =
        "INSERT INTO %1.card_signature VALUES (DEFAULT, :list_id, :name, "
        ":description);";

    query_name_to_sql_command["insert_tag"] =
        "INSERT INTO %1.tag_signature VALUES (DEFAULT, :name);";

    query_name_to_sql_command["insert_into_card_to_tags"] =
        "INSERT INTO %1.card_to_tags VALUES (:card_id, :tag_id);";

    query_name_to_sql_command["update_command"] =
        "UPDATE %1.%2 SET %3 = :new_value WHERE id = "
        ":key_value;";

    query_name_to_sql_command["select_user"] =
        "SELECT id, name FROM %1.user_signature WHERE id = :key_value;";

    query_name_to_sql_command["select_board"] =
        "SELECT id, user_id, name, description FROM %1.board_signature WHERE id"
        " = :key_value;";

    query_name_to_sql_command["select_list"] =
        "SELECT id, board_id, name, description FROM %1.list_signature "
        "WHERE id = :key_value;";

    query_name_to_sql_command["select_card"] =
        "SELECT id, list_id, name, description FROM %1.card_signature "
        "WHERE id = :key_value;";

    query_name_to_sql_command["select_tag"] =
        "SELECT id, name FROM %1.tag_signature WHERE id = :key_value;";

    query_name_to_sql_command["delete_command"] =
        "DELETE FROM %1.%2 WHERE id = :key_value;";

    query_name_to_sql_command["delete_from_card_to_tags"] =
        "DELETE FROM %1.card_to_tags WHERE card_id = :card_id AND tag_id = "
        ":tag_id;";

    query_name_to_sql_command["create_schema"] =
        "CREATE SCHEMA %1;"
        "DO $$DECLARE tbl_record RECORD;"
        "BEGIN"
        "FOR tbl_record IN (SELECT %1 FROM pg_tables WHERE schemaname = "
        "'project_template') LOOP"
        "EXECUTE 'CREATE TABLE %1.' || tbl_record.tablename || ' (LIKE "
        "project_template.' || tbl_record.tablename || ' INCLUDING "
        "CONSTRAINTS)';"
        "END LOOP;"
        "END$$;";

    query_name_to_sql_command["select_subobject_ids"] =
        "SELECT %2 FROM %1.%3 WHERE %4 = :id";

    query_name_to_sql_command["select_last_value"] =
        "SELECT last_value FROM %1";

    query_name_to_sql_command["check_user_rights"] =
        "SELECT exists (SELECT * FROM %1.board_signature WHERE id = :board_id "
        "AND user_id = :user_id LIMIT 1);";

    query_name_to_sql_command["check_user_existence"] =
        "SELECT exists (SELECT * FROM %1.user_authorization_data WHERE login = "
        ":login LIMIT 1);";

    query_name_to_sql_command["check_user_password"] =
        "SELECT exists (SELECT * FROM %1.user_authorization_data WHERE login = "
        ":login AND password = :password LIMIT 1);";

    query_name_to_sql_command["get_user_id_by_name"] =
        "SELECT id FROM %1.user_signature WHERE name = :name;";
    // "SET search_path TO public;";
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
            qDebug() << "drop_all_tables:" << m_database.lastError();
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
            qDebug() << "clear_all_tables" << m_database.lastError();
        }
    }
}

void db_manager::set_schema(const QString &name) {
    m_schema = name;
}

quint32 db_manager::get_sequence_last_value(const QString &sequence) {
    //  TODO: cannot explicitly set a schema
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["select_last_value"].arg(sequence));
    if (!query.exec()) {
        qDebug() << "get_sequence_last_value:" << m_database.lastError();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

quint32 db_manager::get_user_id_by_name(const QString &name) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["get_user_id_by_name"].arg(m_schema)
    );
    query.bindValue(":name", name);
    if (!query.exec()) {
        qDebug() << "get_user_id_by_name:" << m_database.lastError();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

bool db_manager::check_user_password(
    const QString &login,
    const QString &password
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["check_user_password"].arg(m_schema)
    );
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    if (!query.exec()) {
        qDebug() << "check_user_password:" << m_database.lastError();
        return false;
    }
    query.next();
    return query.value(0).toBool();
}

quint32
db_manager::authorize_user(const QString &login, const QString &password) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["check_user_existence"].arg(m_schema
    ));
    query.bindValue(":login", login);
    if (!query.exec()) {
        qDebug() << "authorize_user:" << m_database.lastError();
        return 0;
    }
    query.next();
    if (query.value(0).toBool()) {
        if (check_user_password(login, password)) {
            return get_user_id_by_name(login);
        }
        return 0;
    }
    return insert_user(login, password);
}
bool db_manager::check_user_rights(quint32 user_id, quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["check_user_rights"].arg(m_schema));
    query.bindValue(":user_id", user_id);
    query.bindValue(":board_id", board_id);
    if (!query.exec()) {
        qDebug() << "check_user_rights:" << m_database.lastError();
        return false;
    }
    query.next();
    return query.value(0).toBool();
}

quint32 db_manager::insert_user(const QString &login, const QString &password) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_user_data"].arg(m_schema));
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    if (!query.exec()) {
        qDebug() << "insert_user:" << m_database.lastError();
        return 0;
    }
    query.prepare(
        query_name_to_sql_command["insert_user_signature"].arg(m_schema)
    );
    query.bindValue(":login", login);
    if (!query.exec()) {
        qDebug() << "insert_user:" << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(USER_ID_SEQUENCE);
}

quint32 db_manager::insert_board(
    quint32 user_id,
    const QString &name,
    const QString &description
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_board"].arg(m_schema));
    query.bindValue(":user_id", user_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    if (!query.exec()) {
        qDebug() << "insert_board:" << m_database.lastError();
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
        qDebug() << "insert_list" << m_database.lastError();
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
        qDebug() << "insert_card" << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(CARD_ID_SEQUENCE);
}

quint32 db_manager::insert_tag(const QString &name) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_tag"].arg(m_schema));
    query.bindValue(":name", name);
    if (!query.exec()) {
        qDebug() << "insert_tag" << m_database.lastError();
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
        qDebug() << "pin_tag_to_card:" << m_database.lastError();
        return false;
    }
    return true;
}

bool db_manager::update_command(
    const QString &table_name,
    const QString &updating_field_name,
    const QString &new_value,
    quint32 key_value
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["update_command"].arg(
        m_schema, table_name, updating_field_name
    ));
    query.bindValue(":new_value", new_value);
    query.bindValue(":key_value", key_value);
    if (!query.exec()) {
        qDebug() << "update_command:" << m_database.lastError();
        return false;
    }
    return true;
}

bool db_manager::delete_command(const QString &table_name, quint32 key_value) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["delete_command"].arg(m_schema, table_name)
    );
    query.bindValue(":key_value", key_value);
    if (!query.exec()) {
        qDebug() << "delete_command: " << m_database.lastError();
        return false;
    }
    return true;
}

bool db_manager::unpin_tag_from_card(int card_id, int tag_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["delete_from_card_to_tags"].arg(m_schema)
    );
    query.bindValue(":card_id", card_id);
    query.bindValue(":tag_id", tag_id);
    if (!query.exec()) {
        qDebug() << "unpin_tag_from_card: " << m_database.lastError();
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
        qDebug() << "select_info_by_id:" << m_database.lastError();

    }
    query.next();
    return query.record();
}

user db_manager::select_user(quint32 id) {
    auto data = get_data(select_info_by_id("select_user", id));
    quint32 user_id = data[0].toInt();
    QString name = data[1].toString();
    return user(user_id, name);
}

board db_manager::select_board(quint32 id) {
    auto data = get_data(select_info_by_id("select_board", id));
    quint32 board_id = data[0].toInt();
    quint32 user_id = data[1].toInt();
    QString name = data[2].toString();
    QString description = data[3].toString();
    return board(board_id, user_id, name, description);
}

list db_manager::select_list(quint32 id) {
    auto data = get_data(select_info_by_id("select_list", id));
    quint32 list_id = data[0].toInt();
    quint32 board_id = data[1].toInt();
    QString name = data[2].toString();
    QString description = data[3].toString();
    return list(list_id, board_id, name, description);
}

card db_manager::select_card(quint32 id) {
    auto data = get_data(select_info_by_id("select_card", id));
    quint32 card_id = data[0].toInt();
    quint32 list_id = data[1].toInt();
    QString name = data[2].toString();
    QString description = data[3].toString();
    return card(card_id, list_id, name, description);
}

tag db_manager::select_tag(quint32 id) {
    auto data = get_data(select_info_by_id("select_tag", id));
    quint32 tag_id = data[0].toInt();
    QString name = data[1].toString();
    return tag(tag_id, name);
}

QVector<board> db_manager::get_user_boards(quint32 user_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["select_subobject_ids"].arg(
        m_schema, "id", "board_signature", "user_id"
    ));
    query.bindValue(":id", user_id);
    if (!query.exec()) {
        qDebug() << "get_user_boards:" << m_database.lastError();
    }
    QVector<board> result;
    while (query.next()) {
        result.push_back(select_board(query.value(0).toInt()));
    }
    return result;
}

QVector<list> db_manager::get_board_lists(quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["select_subobject_ids"].arg(
        m_schema, "id", "list_signature", "board_id"
    ));
    query.bindValue(":id", board_id);
    if (!query.exec()) {
        qDebug() << "get_board_lists:" << m_database.lastError();
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
        m_schema, "id", "card_signature", "list_id"
    ));
    query.bindValue(":id", list_id);
    if (!query.exec()) {
        qDebug() << "get_list_cards:" << m_database.lastError();
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
        qDebug() << "get_card_tags:" << m_database.lastError();
    }
    QVector<tag> result;
    while (query.next()) {
        result.push_back(select_tag(query.value(0).toInt()));
    }
    return result;
}

board db_manager::get_full_board(quint32 board_id) {
    board board = select_board(board_id);
    board.m_lists = get_board_lists(board_id);
    for (auto &list : board.m_lists) {
        list.m_cards = get_list_cards(list.m_list_id);
    }
    return board;
}

}  // namespace database
