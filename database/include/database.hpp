#ifndef DATABASE_HPP_
#define DATABASE_HPP_
#include <QVector>
#include <QtCore>
#include <QtSql>
#include "element_classes.hpp"

namespace database {
const QString BOARD_TABLE_NAME = "board_signature";
const QString BOARD_PRIMARY_KEY = "board_id";
const QString LIST_TABLE_NAME = "list_signature";
const QString LIST_PRIMARY_KEY = "list_id";
const QString CARD_TABLE_NAME = "card_signature";
const QString CARD_PRIMARY_KEY = "card_id";
const QString TAG_TABLE_NAME = "tag_signature";
const QString TAG_PRIMARY_KEY = "tag_id";
const QString USER_ID_SEQUENCE = "user_signature_user_id_seq";
const QString BOARD_ID_SEQUENCE = "board_signature_board_id_seq";
const QString LIST_ID_SEQUENCE = "list_signature_list_id_seq";
const QString CARD_ID_SEQUENCE = "card_signature_card_id_seq";
const QString TAG_ID_SEQUENCE = "tag_signature_tag_id_seq";

const QString QT_DATABASE_DRIVER = "QPSQL";

class db_manager {
    QString m_database_name;
    QString m_user_name;
    QString m_host_name;
    QString m_password;
    QSqlDatabase m_database = QSqlDatabase::addDatabase(QT_DATABASE_DRIVER);
    static QVector<QVariant> get_data(const QSqlRecord &record);
    QSqlRecord select_info_by_id(const QString &query_name, quint32 key_value);
    QString m_schema = "public";

public:
    db_manager(
        QString database_name,
        QString user_name,
        QString host_name,
        QString password
    );
    void print_all_tables();
    void clear_all_tables();
    void drop_all_tables();

    void set_schema(const QString &name);
    void test_foo();
//    void create_schema(const QString &schema_name);
    quint32 insert_user(const QString &name);
    quint32 insert_board(quint32 user_id, const QString &name, const QString &description);
    quint32 insert_list(int board_id, const QString &name, const QString &description);
    quint32 insert_card(int list_id, const QString &name, const QString &description);
    quint32 insert_tag(const QString &name);
    bool pin_tag_to_card(int card_id, int tag_id);
    bool update_command(
        const QString &table_name,
        const QString &updating_field_name,
        const QString &key_field_name,
        const QString &new_value,
        quint32 key_value
    );
    board select_board(quint32 id);
    list select_list(quint32 id);
    card select_card(quint32 id);
    tag select_tag(quint32 id);
    bool delete_command(
        const QString &table_name,
        const QString &key_field_name,
        quint32 key_value
    );
    QVector<list> get_board_lists(quint32 board_id);
    QVector<card> get_list_cards(quint32 list_id);
    QVector<tag> get_card_tags(quint32 card_id);

    bool check_user_rights(quint32 user_id, quint32 board_id);
    quint32 get_sequence_last_value(const QString &sequence);
};

QMap<QString, QString> query_name_to_sql_command;

void fill_query_name_to_sql_command() {
    query_name_to_sql_command["insert_user"] =
        "INSERT INTO %1.user_signature VALUES (DEFAULT, :name);";

    query_name_to_sql_command["insert_board"] =
        "INSERT INTO %1.board_signature VALUES (DEFAULT, :board_id, :name, "
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
        "UPDATE %1.%2 SET %3 = :new_value WHERE %4 = "
        ":key_value;";

    query_name_to_sql_command["select_board"] =
        "SELECT board_id, user_id, name, description FROM %1.board_signature WHERE board_id "
        "= "
        ":key_value;";

    query_name_to_sql_command["select_list"] =
        "SELECT list_id, board_id, name, description FROM %1.list_signature "
        "WHERE "
        "list_id = :key_value;";

    query_name_to_sql_command["select_card"] =
        "SELECT card_id, list_id, name, description FROM %1.card_signature "
        "WHERE "
        "card_id = :key_value;";

    query_name_to_sql_command["select_tag"] =
        "SELECT tag_id, name FROM %1.tag_signature WHERE tag_id = :key_value;";

    query_name_to_sql_command["delete_command"] =
        "DELETE FROM %1.%2 WHERE %3 = :key_value;";

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
            "select currval(:sequence_name);";

    query_name_to_sql_command["check_user_rights"] =
            "SELECT exists (SELECT * FROM %1.board_signature WHERE board_id = :board_id AND user_id = :user_id LIMIT 1);";
    // "SET search_path TO public;";
}

/*template<class U>
class abstract_element {
private:
    template<class T>
    std::function<void (T&, QVariant)> set_board_id = [&](T& object, const
QVariant &value){ object.board_id = value.toInt();
    };
    template<class T>
    std::function<void (T&, QVariant)> set_list_id = [&](T& object, const
QVariant &value){ object.list_id = value.toInt();
    };
    template<class T>
    std::function<void (T&, QVariant)> set_card_id = [&](T& object, const
QVariant &value){ object.card_id = value.toInt();
    };
    template<class T>
    std::function<void (T&, QVariant)> set_name = [&](T& object, const QVariant
&value){ object.name = value.toString();
    };
    template<class T>
    std::function<void (T&, QVariant)> set_description = [&](T& object, const
QVariant &value){ object.description = value.toString();
    };
public:
    std::map<QString, std::function<void (T& object, const QVariant &value)>>
field_name_to_func = {
        {"board_id", set_board_id},
        {"name", set_name}
    };

    explicit board(quint32 id) {
    QSqlRecord rec = select_info_by_id("board_signature", "board_id", id);
    for (int i = 0; i < rec.count(); ++i) {
        QSqlField field = rec.field(i);
        abstract_element<board> ae;
        ae.field_name_to_func[field.name()](*this, field.value());
    }
};
}; */

}  // namespace database

#endif  // DATABASE_HPP_