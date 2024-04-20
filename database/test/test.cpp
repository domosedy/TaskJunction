#include <iostream>
#include "database.hpp"
#include "doctest.h"
#include "element_classes.hpp"
#include "test_utils.hpp"

using namespace database;
QVector<QString> arguments = {"postgres", "ivan", "localhost", "1"};

#ifdef DEFAULT_TESTS

TEST_CASE("create") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();
    {
        quint32 id = db_manager.authorize_user("test_user", "test_password");
        CHECK(id == 1);
        auto user = db_manager.select_user(id);
        CHECK(user.m_name == "test_user");
        CHECK(user.m_user_id == id);
    }

    {
        auto groups = db_manager.get_user_groups(1);
        CHECK(groups.size() == 1);
        CHECK(groups[0].m_name == "default");
        CHECK(groups[0].m_group_id == 1);
    }

    {
        quint32 id = db_manager.create_group("test_group");
        CHECK(id == 2);
    }

    {
        quint32 id =
            db_manager.insert_board(1, "test_board", "test description");
        CHECK(id == 1);
    }

    {
        quint32 id = db_manager.insert_list(1, "test_list", "test description");
        CHECK(id == 1);
    }

    {
        quint32 id = db_manager.insert_card(1, "test_card", "test description");
        CHECK(id == 1);
    }

    {
        quint32 id = db_manager.insert_tag("test_tag");
        CHECK(id == 1);
    }
}

TEST_CASE("select") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    auto groups = db_manager.get_user_groups(user_id);
    quint32 group_id = groups[0].m_group_id;
    quint32 board_id =
        db_manager.insert_board(group_id, "test_board", "test description");
    quint32 list_id =
        db_manager.insert_list(board_id, "test_list", "test description");
    quint32 card_id =
        db_manager.insert_card(list_id, "test_card", "test description");
    quint32 tag_id = db_manager.insert_tag("test_tag");

    {
        user user = db_manager.select_user(user_id);
        CHECK(user.m_user_id == user_id);
        CHECK(user.m_name == "test_user");
    }

    {
        group group = db_manager.select_group(group_id);
        CHECK(group.m_group_id == group_id);
        CHECK(group.m_name == "default");
    }

    {
        board board = db_manager.select_board(board_id);
        CHECK(board.m_board_id == board_id);
        CHECK(board.m_name == "test_board");
        CHECK(board.m_group_id == group_id);
        CHECK(board.m_description == "test description");
    }

    {
        list list = db_manager.select_list(list_id);
        CHECK(list.m_list_id == list_id);
        CHECK(list.m_board_id == board_id);
        CHECK(list.m_name == "test_list");
        CHECK(list.m_description == "test description");
    }

    {
        card card = db_manager.select_card(card_id);
        CHECK(card.m_card_id == card_id);
        CHECK(card.m_name == "test_card");
        CHECK(card.m_list_id == list_id);
        CHECK(card.m_description == "test description");
    }

    {
        tag tag = db_manager.select_tag(tag_id);
        CHECK(tag.m_tag_id == tag_id);
        CHECK(tag.m_name == "test_tag");
    }
}

template <typename T>
bool is_equivalent(QVector<T> &lhs, QVector<T> &rhs) {
    if (lhs.size() == rhs.size()) {
        for (int i = 0; i < lhs.size(); ++i) {
            if (lhs[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

// TODO check user/group rights
// TODO check update_command

TEST_CASE("get user groups") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    {
        quint32 user_id =
            db_manager.authorize_user("test_user", "test_password");
        QVector<group> answer = {group(1, "default")};
        auto result = db_manager.get_user_groups(user_id);
        CHECK(is_equivalent(answer, result));
    }

    {
        quint32 user_id =
            db_manager.authorize_user("test_user", "test_password");
        quint32 group_id = db_manager.create_group("test_group");
        db_manager.add_user_to_group(user_id, group_id);
        QVector<group> answer = {group(1, "default"), group(2, "test_group")};
        auto result = db_manager.get_user_groups(user_id);
        CHECK(is_equivalent(answer, result));
    }

    {
        quint32 user_id =
            db_manager.authorize_user("test_user", "test_password");
        CHECK(db_manager.delete_user_from_group(user_id, 2));
        QVector<group> answer = {group(1, "default")};
        auto result = db_manager.get_user_groups(user_id);
        CHECK(is_equivalent(answer, result));
    }
}

TEST_CASE("update order") {
    db_manager db_manager(
            arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    {
        quint32 user_id = db_manager.authorize_user("test_user", "test_password");
        quint32 group_id = db_manager.get_user_groups(user_id)[0].m_group_id;
        quint32 board_id = db_manager.insert_board(group_id, "test_board", "");
        quint32 list_id_1 = db_manager.insert_list(board_id, "test_list_1", "");
        quint32 list_id_2 = db_manager.insert_list(board_id, "test_list_2", "");
        CHECK(db_manager.update_order("list_signature", list_id_1, 2));
        CHECK(db_manager.get_number("list_signature", list_id_1) == 2);
        CHECK(db_manager.get_number("list_signature", list_id_2) == 1);

        quint32 list_id_3 = db_manager.insert_list(board_id, "test_list_3", "");
        CHECK(db_manager.update_order("list_signature", list_id_3, 1));
        CHECK(db_manager.get_number("list_signature", list_id_1) == 3);
        CHECK(db_manager.get_number("list_signature", list_id_2) == 2);
        CHECK(db_manager.get_number("list_signature", list_id_3) == 1);

        CHECK(db_manager.update_order("list_signature", list_id_3, 3));
        CHECK(db_manager.get_number("list_signature", list_id_1) == 2);
        CHECK(db_manager.get_number("list_signature", list_id_2) == 1);
        CHECK(db_manager.get_number("list_signature", list_id_3) == 3);
    }

    {
        quint32 user_id = db_manager.authorize_user("test_user", "test_password");
        quint32 group_id = db_manager.get_user_groups(user_id)[0].m_group_id;
        quint32 board_id = db_manager.insert_board(group_id, "test_board", "");
        quint32 list_id_1 = db_manager.insert_list(board_id, "test_list_1", "");
        CHECK(!db_manager.update_order("list_signature", list_id_1, 100));
        CHECK(!db_manager.update_order("list_signature", list_id_1, 0));
}
}

TEST_CASE("add user to group / get group users id") {
    db_manager db_manager(
            arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id_1 = db_manager.authorize_user("test_user_1", "");
    quint32 user_id_2 = db_manager.authorize_user("test_user_2", "");
    quint32 user_id_3 = db_manager.authorize_user("test_user_3", "");
    quint32 group_id = db_manager.create_group("test_group");

    db_manager.add_user_to_group(user_id_1, group_id);
    db_manager.add_user_to_group(user_id_2, group_id);
    db_manager.add_user_to_group(user_id_3, group_id);

    QVector<quint32> answer{user_id_1, user_id_2, user_id_3};
    auto result = db_manager.get_group_users_id(group_id);

    CHECK(is_equivalent(result, answer));
}

#endif

#ifdef TEST_NEW_FEATURE

TEST_CASE("new feature") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();
}

#endif
