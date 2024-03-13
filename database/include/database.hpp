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

const QString QT_DATABASE_DRIVER = "QPSQL";

class db_accessor {
    QString m_database_name;
    QString m_user_name;
    QString m_host_name;
    QString m_password;
    QSqlDatabase m_database = QSqlDatabase::addDatabase(QT_DATABASE_DRIVER);

public:
    db_accessor(
        QString database_name,
        QString user_name,
        QString host_name,
        QString password
    );
    void print_all_tables();
    void clear_all_tables();
    void drop_all_tables();
};

class db_manager {
private:
    static QVector<QVariant> get_data(const QSqlRecord &record);
    static QSqlRecord select_info_by_id(
        const QString &table_name,
        const QString &primary_key,
        unsigned key_value
    );

public:
    static void insert_board(const QString &name);
    static void
    insert_list(int board_id, const QString &name, const QString &description);
    static void
    insert_card(int list_id, const QString &name, const QString &description);
    static void insert_tag(const QString &name);
    static void pin_tag_to_card(int card_id, int tag_id);
    static void update_command(
        const QString &table_name,
        const QString &updating_field_type,
        const QString &updating_field_name,
        const QString &key_field_name,
        const QString &new_value,
        unsigned key_value
    );
    static board select_board(unsigned id);
    static list select_list(unsigned id);
    static card select_card(unsigned id);
    static tag select_tag(unsigned id);
    static void delete_command(
        const QString &table_name,
        const QString &key_field_name,
        unsigned key_value
    );
};

QMap<QString, QString> query_name_to_sql_command;

void fill_query_name_to_sql_command() {
    query_name_to_sql_command["insert_board"] = "SELECT insert_board(:name)";

    query_name_to_sql_command["insert_list"] =
        "SELECT insert_list(:board_id, :name, :description)";

    query_name_to_sql_command["insert_card"] =
        "SELECT insert_card(:list_id, :name, :description)";

    query_name_to_sql_command["insert_tag"] = "SELECT insert_tag(:name)";

    query_name_to_sql_command["insert_into_card_to_tags"] =
        "SELECT insert_into_card_to_tags(:card_id, :tag_id)";

    query_name_to_sql_command["create_update_function"] =
        "CREATE FUNCTION update_%1_%4(%2, integer) RETURNS void AS $$"
        "UPDATE %1 SET %4 = $1 WHERE %5 = $2;"
        "$$ LANGUAGE SQL;";
    query_name_to_sql_command["update_function"] = "SELECT update_%1_%4(?, ?)";

    query_name_to_sql_command["select_info_by_id"] =
        "SELECT * FROM %1 WHERE %2 = %3;";

    query_name_to_sql_command["create_delete_function"] =
        "CREATE FUNCTION delete_from_%1(integer) RETURNS void AS $$"
        "DELETE FROM %1 WHERE %2 = $1;"
        "$$ LANGUAGE SQL;";
    query_name_to_sql_command["delete_function"] = "SELECT delete_from_%1(?)";
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

    explicit board(unsigned id) {
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
