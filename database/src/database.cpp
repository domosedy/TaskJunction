#include "database.hpp"
#include "logging.hpp"
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
        rDebug() << "Cannot open database:" << m_database.lastError();
    }
    fill_query_name_to_sql_command();
}

QMap<QString, QString> db_manager::query_name_to_sql_command;

void db_manager::fill_query_name_to_sql_command() {
    query_name_to_sql_command["insert_user_data"] =
        "INSERT INTO %1.user_authorization_data VALUES (:login, :password);";

    query_name_to_sql_command["insert_user_signature"] =
        "INSERT INTO %1.user_signature VALUES (DEFAULT, :login);";

    query_name_to_sql_command["create_group"] =
        "INSERT INTO %1.group_signature VALUES (DEFAULT, :name);";

    query_name_to_sql_command["add_user_to_group"] =
        "INSERT INTO %1.user_to_group VALUES (:user_id, :group_id);";

    query_name_to_sql_command["insert_board"] =
        "INSERT INTO %1.board_signature VALUES (DEFAULT, :group_id, :name, "
        ":description, DEFAULT);";

    query_name_to_sql_command["insert_list"] =
        "INSERT INTO %1.list_signature VALUES (DEFAULT, :board_id, :name, "
        ":description, DEFAULT);";

    query_name_to_sql_command["insert_card"] =
        "INSERT INTO %1.card_signature VALUES (DEFAULT, :list_id, :name, "
        ":description, DEFAULT);";

    query_name_to_sql_command["insert_tag"] =
        "INSERT INTO %1.tag_signature VALUES (DEFAULT, :name);";

    query_name_to_sql_command["insert_into_card_to_tags"] =
        "INSERT INTO %1.card_to_tags VALUES (:card_id, :tag_id);";

    query_name_to_sql_command["update_command"] =
        "UPDATE %1.%2 SET %3 = :new_value WHERE id = "
        ":key_value;";

    query_name_to_sql_command["update_order"] =
        "UPDATE %1.%2 SET number = number + (%3) WHERE number > :left AND "
        "number < :right;";

    query_name_to_sql_command["select_group"] =
        "SELECT id, name FROM %1.group_signature WHERE id = :key_value;";

    query_name_to_sql_command["select_user"] =
        "SELECT id, name FROM %1.user_signature WHERE id = :key_value;";

    query_name_to_sql_command["select_board"] =
        "SELECT id, group_id, name, description FROM %1.board_signature WHERE "
        "id"
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

    query_name_to_sql_command["delete_user_from_group"] =
        "DELETE FROM %1.user_to_group WHERE user_id = :user_id AND "
        "group_id = :group_id;";

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

    query_name_to_sql_command["select_group_ids_by_user_id"] =
        "SELECT group_id FROM %1.user_to_group WHERE user_id = :id;";

    query_name_to_sql_command["select_board_ids_by_group_id"] =
        "SELECT id FROM %1.board_signature WHERE group_id = :id "
        "ORDER BY %1.board_signature.number;";

    query_name_to_sql_command["select_list_ids_by_board_id"] =
        "SELECT id FROM %1.list_signature WHERE board_id = :id;";

    query_name_to_sql_command["select_card_ids_by_list_id"] =
        "SELECT id FROM %1.card_signature WHERE list_id = :id;";

    query_name_to_sql_command["select_tag_ids_by_card_id"] =
        "SELECT tag_id FROM %1.card_to_tags WHERE tag_id = :id;";

    query_name_to_sql_command["select_last_value"] =
        "SELECT last_value FROM %1";

    query_name_to_sql_command["select_group_id_by_board_id"] =
        "SELECT group_id FROM %1.board_signature WHERE id = :board_id;";

    query_name_to_sql_command["check_group_rights"] =
        "SELECT exists (SELECT * FROM %1.board_signature WHERE id = :board_id "
        "AND group_id = :group_id LIMIT 1);";

    query_name_to_sql_command["check_user_rights"] =
        "SELECT exists (SELECT * FROM %1.user_to_group WHERE user_id = "
        ":user_id AND group_id = :group_id);";

    query_name_to_sql_command["check_user_existence"] =
        "SELECT exists (SELECT * FROM %1.user_authorization_data WHERE login = "
        ":login LIMIT 1);";

    query_name_to_sql_command["check_user_password"] =
        "SELECT exists (SELECT * FROM %1.user_authorization_data WHERE login = "
        ":login AND password = :password LIMIT 1);";

    query_name_to_sql_command["get_user_id_by_name"] =
        "SELECT id FROM %1.user_signature WHERE name = :name;";

    query_name_to_sql_command["get_number"] =
        "SELECT number FROM %1.%2 WHERE id = :id;";

    query_name_to_sql_command["alter_sequence"] =
        "SELECT setval('%1.%2', %3, FALSE);";
    //        "ALTER SEQUENCE %1.%2 RESTART WITH %3;";

    query_name_to_sql_command["get_rows_number"] =
        "SELECT count(*) FROM %1.%2;";
    // "SET search_path TO public;";
}

void db_manager::drop_all_tables() {
    QStringList all_tables = m_database.tables();
    QString command = "DROP TABLE %1 CASCADE;";
    foreach (QString str, all_tables) {
        QSqlQuery query(m_database);
        if (!query.exec(command.arg(str))) {
            rDebug() << "drop_all_tables:" << m_database.lastError();
        }
    }
}

void db_manager::print_all_tables() {
    QStringList all_tables = m_database.tables();
    foreach (QString str, all_tables) {
        rDebug() << "Table:" << str;
    }
}

void db_manager::clear_all_tables() {  // TODO reset sequences
    QStringList all_tables = m_database.tables();
    QString command = "DELETE FROM %1;";
    foreach (QString str, all_tables) {
        QSqlQuery query(m_database);
        if (!query.exec(command.arg(str))) {
            rDebug() << "clear_all_tables" << m_database.lastError();
        }
    }

    for (auto &sequence_name : sequences_names) {
        QSqlQuery query(m_database);
        query.prepare(query_name_to_sql_command["alter_sequence"].arg(
            m_schema, sequence_name, "1"
        ));
        if (!query.exec()) {
            rDebug() << "clear_all_tables::alter_sequence"
                     << m_database.lastError() << '\n'
                     << query_name_to_sql_command["alter_sequence"].arg(
                            m_schema, sequence_name, "1"
                        );
        }
    }
}

void db_manager::set_schema(const QString &name) {
    m_schema = name;
}

quint32 db_manager::get_sequence_last_value(const QString &sequence) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["select_last_value"].arg(sequence));
    if (!query.exec()) {
        rDebug() << "get_sequence_last_value:" << m_database.lastError();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

int db_manager::get_rows_number(const QString &table_name) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["get_rows_number"].arg(m_schema, table_name));
    if (!query.exec()) {
        rDebug() << "get_rows_number" << m_database.lastError();
        return -1;
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
        rDebug() << "get_user_id_by_name:" << m_database.lastError();
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
        rDebug() << "check_user_password:" << m_database.lastError();
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
        rDebug() << "authorize_user:" << m_database.lastError();
        return 0;
    }
    query.next();
    if (query.value(0).toBool()) {
        if (check_user_password(login, password)) {
            return get_user_id_by_name(login);
        }
        return 0;
    }
    quint32 user_id = insert_user(login, password);
    quint32 group_id = create_group("default");
    add_user_to_group(user_id, group_id);

    // FIXME: delete this string after testing http + ssl connection
    add_user_to_group(user_id, 1);
    return user_id;
}

quint32 db_manager::get_group_id_by_board_id(const quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["select_group_id_by_board_id"].arg(m_schema)
    );
    query.bindValue(":board_id", board_id);
    if (!query.exec()) {
        rDebug() << "get_group_id_by_board_id:" << m_database.lastError();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

bool db_manager::check_user_rights(quint32 user_id, quint32 board_id) {
    quint32 group_id = get_group_id_by_board_id(board_id);
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["check_user_rights"].arg(m_schema));
    query.bindValue(":user_id", user_id);
    query.bindValue(":group_id", group_id);
    if (!query.exec()) {
        rDebug() << "check_user_rights:" << m_database.lastError();
        return false;
    }
    query.next();
    return query.value(0).toBool();
}

bool db_manager::check_group_rights(quint32 group_id, quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["check_group_rights"].arg(m_schema)
    );
    query.bindValue(":group_id", group_id);
    query.bindValue(":board_id", board_id);
    if (!query.exec()) {
        rDebug() << "check_group_rights:" << m_database.lastError();
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
        rDebug() << "insert_user:" << m_database.lastError();
        return 0;
    }
    query.prepare(
        query_name_to_sql_command["insert_user_signature"].arg(m_schema)
    );
    query.bindValue(":login", login);
    if (!query.exec()) {
        rDebug() << "insert_user:" << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(USER_ID_SEQUENCE);
}

quint32 db_manager::create_group(const QString &name) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["create_group"].arg(m_schema));
    query.bindValue(":name", name);
    if (!query.exec()) {
        rDebug() << "create_group:" << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(GROUP_ID_SEQUENCE);
}

bool db_manager::add_user_to_group(quint32 user_id, quint32 group_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["add_user_to_group"].arg(m_schema));
    query.bindValue(":user_id", user_id);
    query.bindValue(":group_id", group_id);
    if (!query.exec()) {
        rDebug() << "add_user_to_group:" << m_database.lastError();
        return false;
    }
    return true;
}

quint32 db_manager::insert_board(
    quint32 group_id,
    const QString &name,
    const QString &description
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_board"].arg(m_schema));
    query.bindValue(":group_id", group_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    if (!query.exec()) {
        rDebug() << "insert_board:" << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(BOARD_ID_SEQUENCE);
}

quint32 db_manager::insert_list(
    quint32 board_id,
    const QString &name,
    const QString &description
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_list"].arg(m_schema));
    query.bindValue(":board_id", board_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    if (!query.exec()) {
        rDebug() << "insert_list" << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(LIST_ID_SEQUENCE);
}

quint32 db_manager::insert_card(
    quint32 list_id,
    const QString &name,
    const QString &description
) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_card"].arg(m_schema));
    query.bindValue(":list_id", list_id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    if (!query.exec()) {
        rDebug() << "insert_card" << m_database.lastError();
        return 0;
    }
    return get_sequence_last_value(CARD_ID_SEQUENCE);
}

quint32 db_manager::insert_tag(const QString &name) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_tag"].arg(m_schema));
    query.bindValue(":name", name);
    if (!query.exec()) {
        rDebug() << "insert_tag" << m_database.lastError();
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
        rDebug() << "pin_tag_to_card:" << m_database.lastError();
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
        rDebug() << "update_command:" << m_database.lastError();
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
        rDebug() << "delete_command: " << m_database.lastError();
        return false;
    }
    return true;
}

bool db_manager::delete_user_from_group(quint32 user_id, quint32 group_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["delete_user_from_group"].arg(m_schema)
    );
    query.bindValue(":user_id", user_id);
    query.bindValue(":group_id", group_id);
    if (!query.exec()) {
        rDebug() << "delete_user_from_group" << m_database.lastError();
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
        rDebug() << "unpin_tag_from_card: " << m_database.lastError();
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
        rDebug() << "select_info_by_id:" << m_database.lastError();
    }
    query.next();
    return query.record();
}

group db_manager::select_group(quint32 id) {
    auto data = get_data(select_info_by_id("select_group", id));
    quint32 group_id = data[0].toInt();
    QString name = data[1].toString();
    return group(group_id, name);
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
    quint32 group_id = data[1].toInt();
    QString name = data[2].toString();
    QString description = data[3].toString();
    return board(board_id, group_id, name, description);
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

QVector<group> db_manager::get_user_groups(quint32 user_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["select_group_ids_by_user_id"].arg(m_schema)
    );
    query.bindValue(":id", user_id);
    if (!query.exec()) {
        rDebug() << "get_user_groups:" << m_database.lastError();
    }
    QVector<group> result;
    while (query.next()) {
        result.push_back(select_group(query.value(0).toInt()));
    }
    return result;
}

QVector<board> db_manager::get_group_boards(quint32 group_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["select_board_ids_by_group_id"].arg(m_schema)
    );
    query.bindValue(":id", group_id);
    if (!query.exec()) {
        rDebug() << "get_user_boards:" << m_database.lastError();
    }
    QVector<board> result;
    while (query.next()) {
        result.push_back(select_board(query.value(0).toInt()));
    }
    return result;
}

QVector<list> db_manager::get_board_lists(quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["select_list_ids_by_board_id"].arg(m_schema)
    );
    query.bindValue(":id", board_id);
    if (!query.exec()) {
        rDebug() << "get_board_lists:" << m_database.lastError();
    }
    QVector<list> result;
    while (query.next()) {
        result.push_back(select_list(query.value(0).toInt()));
    }
    return result;
}

QVector<card> db_manager::get_list_cards(quint32 list_id) {
    QSqlQuery query(m_database);

    query.prepare(
        query_name_to_sql_command["select_card_ids_by_list_id"].arg(m_schema)
    );
    query.bindValue(":id", list_id);
    if (!query.exec()) {
        rDebug() << "get_list_cards:" << m_database.lastError();
    }
    QVector<card> result;
    while (query.next()) {
        result.push_back(select_card(query.value(0).toInt()));
    }
    return result;
}

QVector<tag> db_manager::get_card_tags(quint32 card_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["select_tag_ids_by_card_id"].arg(m_schema)
    );
    query.bindValue(":id", card_id);
    if (!query.exec()) {
        rDebug() << "get_card_tags:" << m_database.lastError();
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

quint32 db_manager::get_number(const QString &table_name, quint32 id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["get_number"].arg(m_schema, table_name)
    );
    query.bindValue(":id", id);
    if (!query.exec()) {
        rDebug() << "get_number:" << m_database.lastError();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

bool db_manager::update_order(
    const QString &table_name,
    quint32 id,
    quint32 new_number
) {
    if (new_number == 0 || new_number > get_rows_number(table_name)) {
        return false;
    }
    quint32 number = get_number(table_name, id);
    if (number == new_number) {
        return true;
    }
    QSqlQuery query(m_database);
    if (new_number < number) {
        query.prepare(query_name_to_sql_command["update_order"].arg(
            m_schema, table_name, "1"
        ));
        query.bindValue(":left", new_number - 1);
        query.bindValue(":right", number);
    } else {
        query.prepare(query_name_to_sql_command["update_order"].arg(
            m_schema, table_name, "-1"
        ));
        query.bindValue(":left", number);
        query.bindValue(":right", new_number + 1);
    }
    if (!query.exec()) {
        rDebug() << "update_order:" << m_database.lastError();
        return false;
    }
    return update_command(
        table_name, "number", QString::number(new_number), id
    );
}

}  // namespace database
