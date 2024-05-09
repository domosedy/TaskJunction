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
    query_name_to_sql_command["authorize_user"] =
        "SELECT %1.authorize_user(:login, :password);";

    query_name_to_sql_command["add_user_to_board"] =
        "INSERT INTO %1.user_to_board VALUES (:user_id, :board_id);";

    query_name_to_sql_command["insert_board"] =
        "SELECT %1.insert_board(:user_id, :name, :description)";

    query_name_to_sql_command["insert_list"] =
        "SELECT %1.insert_list(:board_id, :name, :description)";

    query_name_to_sql_command["insert_card"] =
        "SELECT %1.insert_card(:list_id, :name, :description)";

    query_name_to_sql_command["insert_tag"] =
        "SELECT %1.insert_tag(:name);";

    query_name_to_sql_command["insert_into_card_to_tags"] =
        "INSERT INTO %1.card_to_tags VALUES (:card_id, :tag_id);";

    query_name_to_sql_command["update_command"] =
        "UPDATE %1.%2 SET %3 = :new_value WHERE id = "
        ":key_value;";

    query_name_to_sql_command["move_card"] =
        "SELECT %1.move_card(:id, :new_list_id, :new_number);";

    query_name_to_sql_command["select_user"] =
        "SELECT id, login FROM %1.user_signature WHERE id = :key_value;";

    query_name_to_sql_command["select_board"] =
        "SELECT id, user_id, name, description FROM %1.board_signature WHERE "
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

    query_name_to_sql_command["delete_card"] =
            "SELECT %1.delete_card(:id);";

    query_name_to_sql_command["delete_from_card_to_tags"] =
        "DELETE FROM %1.card_to_tags WHERE card_id = :card_id AND tag_id = "
        ":tag_id;";

    query_name_to_sql_command["delete_user_from_board"] =
        "DELETE FROM %1.user_to_board WHERE user_id = :user_id AND "
        "board_id = :board_id;";

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

    query_name_to_sql_command["select_board_ids_by_user_id"] =
        "SELECT board_id FROM %1.user_to_board WHERE user_id = :id;";
    // TODO "ORDER BY %1.board_signature.number;";

    query_name_to_sql_command["select_list_ids_by_board_id"] =
        "SELECT id FROM %1.list_signature WHERE board_id = :id;";

    query_name_to_sql_command["select_card_ids_by_list_id"] =
        "SELECT id FROM %1.card_signature WHERE list_id = :id;";

    query_name_to_sql_command["select_tag_ids_by_card_id"] =
        "SELECT tag_id FROM %1.card_to_tags WHERE tag_id = :id;";

    query_name_to_sql_command["check_user_rights"] =
        "SELECT exists (SELECT * FROM %1.user_to_board WHERE user_id = "
        ":user_id AND board_id = :board_id);";

    query_name_to_sql_command["get_user_id_by_name"] =
        "SELECT id FROM %1.user_signature WHERE name = :name;";

    query_name_to_sql_command["get_card_number"] =
        "SELECT number FROM %1.card_signature WHERE id = :id;";

    query_name_to_sql_command["alter_sequence"] =
        "SELECT setval('%1.%2', %3, FALSE);";

    query_name_to_sql_command["get_board_users_id"] =
        "SELECT user_id FROM %1.user_to_board WHERE board_id = :board_id;";
}

void db_manager::drop_all_tables() {
    QStringList all_tables = m_database.tables();
    QString command = "DROP TABLE %1 CASCADE;";
    foreach (QString str, all_tables) {
        QSqlQuery query(m_database);
        if (!query.exec(command.arg(str))) {
            qDebug() << "drop_all_tables:" << query.lastError().text();
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
            qDebug() << "clear_all_tables:" << query.lastError().text();
        }
    }

    for (auto &sequence_name : sequences_names) {
        QSqlQuery query(m_database);
        query.prepare(query_name_to_sql_command["alter_sequence"].arg(
            m_schema, sequence_name, "1"
        ));
        if (!query.exec()) {
            qDebug() << "clear_all_tables::alter_sequence"
                     << query.lastError().text() << '\n'
                     << query_name_to_sql_command["alter_sequence"].arg(
                            m_schema, sequence_name, "1"
                        );
        }
    }
}

void db_manager::set_schema(const QString &name) {
    m_schema = name;
}

quint32
db_manager::authorize_user(const QString &login, const QString &password) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["authorize_user"].arg(m_schema));
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    if (!query.exec()) {
        qDebug() << "authorize_user:" << query.lastError().text();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

bool db_manager::check_user_rights(quint32 user_id, quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["check_user_rights"].arg(m_schema));
    query.bindValue(":user_id", user_id);
    query.bindValue(":board_id", board_id);
    if (!query.exec()) {
        qDebug() << "check_user_rights:" << query.lastError().text();
        return false;
    }
    query.next();
    return query.value(0).toBool();
}

bool db_manager::add_user_to_board(quint32 user_id, quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["add_user_to_board"].arg(m_schema));
    query.bindValue(":user_id", user_id);
    query.bindValue(":board_id", board_id);
    if (!query.exec()) {
        qDebug() << "add_user_to_board:" << query.lastError().text();
        return false;
    }
    return true;
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
        qDebug() << "insert_board:" << query.lastError().text();
        return 0;
    }
   query.next();
    return query.value(0).toInt();
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
        qDebug() << "insert_list" << query.lastError().text();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
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
        qDebug() << "insert_card" << query.lastError().text();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

quint32 db_manager::insert_tag(const QString &name) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["insert_tag"].arg(m_schema));
    query.bindValue(":name", name);
    if (!query.exec()) {
        qDebug() << "insert_tag" << query.lastError().text();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

bool db_manager::add_tag_to_card(quint32 card_id, quint32 tag_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["insert_into_card_to_tags"].arg(m_schema)
    );
    query.bindValue(":card_id", card_id);
    query.bindValue(":tag_id", tag_id);
    if (!query.exec()) {
        qDebug() << "add_tag_to_card:" << query.lastError().text();
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
        qDebug() << "update_command:" << query.lastError().text();
        return false;
    }
    return true;
}

bool db_manager::move_card(int id, int new_list_id, int new_number) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["move_card"].arg(m_schema));
    query.bindValue(":id", id);
    query.bindValue(":new_list_id", new_list_id);
    query.bindValue(":new_number", new_number);
    if (!query.exec()) {
        qDebug() << "move_card:" << query.lastError().text();
        return false;
    }
    query.next();
    return query.value(0).toBool();
}

bool db_manager::delete_command(const QString &table_name, quint32 key_value) {
    QSqlQuery query(m_database);
    query.prepare(
            query_name_to_sql_command["delete_command"].arg(m_schema, table_name)
    );
    query.bindValue(":key_value", key_value);
    if (!query.exec()) {
        qDebug() << "delete_command: " << query.lastError().text();
        return false;
    }
    return true;
}

bool db_manager::delete_user(quint32 id) {
    return delete_command(USER_TABLE_NAME, id);
}

bool db_manager::delete_board(quint32 id) {
    return delete_command(BOARD_TABLE_NAME, id);
}

bool db_manager::delete_list(quint32 id) {
    return delete_command(LIST_TABLE_NAME, id);
}

bool db_manager::delete_card(quint32 id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["delete_card"].arg(m_schema));
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << "delete_card:" << query.lastError().text();
        return false;
    }
    return true;
}

bool db_manager::delete_tag(quint32 id) {
    return delete_command(TAG_TABLE_NAME, id);
}

bool db_manager::delete_user_from_board(quint32 user_id, quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["delete_user_from_board"].arg(m_schema)
    );
    query.bindValue(":user_id", user_id);
    query.bindValue(":board_id", board_id);
    if (!query.exec()) {
        qDebug() << "delete_user_from_board" << query.lastError().text();
        return false;
    }
    return true;
}

bool db_manager::delete_tag_from_card(quint32 card_id, quint32 tag_id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["delete_from_card_to_tags"].arg(m_schema)
    );
    query.bindValue(":card_id", card_id);
    query.bindValue(":tag_id", tag_id);
    if (!query.exec()) {
        qDebug() << "delete_tag_from_card: " << query.lastError().text();
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
        qDebug() << "select_info_by_id:" << query.lastError().text();
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
    query.prepare(
        query_name_to_sql_command["select_board_ids_by_user_id"].arg(m_schema)
    );
    query.bindValue(":id", user_id);
    if (!query.exec()) {
        qDebug() << "get_user_boards:" << query.lastError().text();
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
        qDebug() << "get_board_lists:" << query.lastError().text();
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
        qDebug() << "get_list_cards:" << query.lastError().text();
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
        qDebug() << "get_card_tags:" << query.lastError().text();
    }
    QVector<tag> result;
    while (query.next()) {
        result.push_back(select_tag(query.value(0).toInt()));
    }
    return result;
}

QVector<quint32> db_manager::get_board_users_id(quint32 board_id) {
    QSqlQuery query(m_database);
    query.prepare(query_name_to_sql_command["get_board_users_id"].arg(m_schema)
    );
    query.bindValue(":board_id", board_id);
    if (!query.exec()) {
        qDebug() << "get_board_users_id" << query.lastError().text();
        return {};
    }
    QVector<quint32> result;
    while (query.next()) {
        result.push_back(query.value(0).toInt());
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

quint32 db_manager::get_card_number(quint32 id) {
    QSqlQuery query(m_database);
    query.prepare(
        query_name_to_sql_command["get_card_number"].arg(m_schema)
    );
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << "get_card_number:" << query.lastError().text();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

}  // namespace database
