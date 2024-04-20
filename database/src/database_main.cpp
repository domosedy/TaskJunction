#include <QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <iostream>
#include <string>
#include "database.hpp"

int main(int argc, char *argv[]) {
    if (argc < 5) {
        qDebug() << "wrong arguments format";
        return 1;
    }
    using namespace database;
    db_manager db_manager(argv[1], argv[2], argv[3], argv[4]);
    //    db_manager::fill_query_name_to_sql_command();
    //        db_manager.set_schema("test_schema");
    //    fill_query_name_to_sql_command();
    //    db_manager.insert_user("username");
    //    std::cout << std::boolalpha << db_manager.check_user_rights(2, 2);

//    qDebug() << db_manager.authorize_user("username", "password");
//    qDebug() << db_manager.authorize_user("new_username", "new_password");
//    db_manager.create_group("second group");
//    db_manager.add_user_to_group(3, 2);
//    db_manager.add_user_to_group(2, 2);
//    auto res = db_manager.get_user_groups(4);
//    for (auto group: res)
//        group.print_data();
//    for (int i = 1; i < 6; ++i) {
//        db_manager.insert_board(1, "board_name", "description");
//        db_manager.insert_list(1, "list_name", "test3");
//        db_manager.insert_card(1, "card_name", "description");
//        db_manager.insert_tag("tag_name");
//        db_manager.pin_tag_to_card(i, i);
//    }

//    db_manager.update_command("board_signature", "number", "2", 1);
//    db_manager.update_command("board_signature", "number", "1", 2);

    //    db_manager.unpin_tag_from_card(10, 1);
    //    board board(db_manager.select_board(1));
    //    auto lists(db_manager.get_board_lists(1));
    //    for (const auto &list : lists) {
    //        list.print_data();
    //    }
    //        db_manager.update_command(
    //            LIST_TABLE_NAME,  "board_id", LIST_PRIMARY_KEY, "10",2
    //        );
    //    user user = db_manager.select_user(10);
    //    user.print_data();
    //    board board = db_manager.select_board(1);
    //    db_manager.delete_command(LIST_TABLE_NAME, LIST_PRIMARY_KEY, 3);
    //    db_manager.delete_command("list_signature", "list_id", 4);
    //    db_manager.delete_command("user_signature", "user_id", 1);
    //    db_manager.delete_command("card_signature", 1);
//    board board = db_manager.get_full_board(2);
//    for (const auto &list : board.m_lists) {
//        qDebug() << "list:" << list.m_list_id;
//        for (const auto &card : list.m_cards) {
//            qDebug() << "    card:" << card.m_card_id;
//        }
//    }

//    db_manager.update_order("board_signature", 5, 1);

//    qDebug() << db_manager.create_group("first group");
//    qDebug() << db_manager.add_user_to_group(1, 1);

//    db_manager.drop_all_tables();
//    db_manager.clear_all_tables();
    return 0;
}
