#include "doctest.h"
#include "database.hpp"
#include "element_classes.hpp"
#include "test_utils.hpp"
#include <iostream>

using namespace database;
QVector<QString> arguments = {"postgres", "ivan", "localhost", "1"};

// TODO function to reset sequence values
void reset_sequence();

#ifdef DEFAULT_TESTS

TEST_CASE("create") {
    db_manager db_manager(arguments[0], arguments[1], arguments[2], arguments[3]);
    db_manager.clear_all_tables();

    SUBCASE("user") {
        quint32 id = db_manager.authorize_user("test_user", "test_password");
        CHECK(id == 1);
        auto user = db_manager.select_user(id);
        CHECK(user.m_name == "test_user");
        CHECK(user.m_user_id == id);
    }

    SUBCASE("default_group") {
        auto groups = db_manager.get_user_groups(1);
        CHECK(groups.size() == 1);
        CHECK(groups[1].m_name == "default");
        CHECK(groups[1].m_group_id == 1);
    }

    SUBCASE("group") {
        quint32 id = db_manager.create_group("test_group");
        CHECK(id == 2);
    }

    SUBCASE("board") {
        quint32 id = db_manager.insert_board(1, "test_board", "test description");
        CHECK(id == 1);
    }

    SUBCASE("list") {
        quint32 id = db_manager.insert_list(1, "test_list", "test description");
        CHECK(id == 1);
    }

    SUBCASE("card") {
        quint32 id = db_manager.insert_card(1, "test_card", "test description");
        CHECK(id == 1);
    }

    SUBCASE("tag") {
        quint32 id = db_manager.insert_tag("test_tag");
        CHECK(id == 1);
    }
}

TEST_CASE("select") {
    db_manager db_manager(arguments[1], arguments[2], arguments[3], arguments[4]);
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    auto groups = db_manager.get_user_groups(user_id);
    quint32 group_id = groups[1].m_group_id;
    quint32 board_id = db_manager.insert_board(group_id, "test_board", "test description");
    quint32 list_id = db_manager.insert_list(board_id, "test_list", "test description");
    quint32 card_id = db_manager.insert_card(list_id, "test_card", "test description");
    quint32 tag_id = db_manager.insert_tag("test_tag");

    user user = db_manager.select_user(user_id);
    CHECK(user.m_user_id == user_id);
    CHECK(user.m_name == "test_user");

    group group = db_manager.select_group(group_id);
    CHECK(group.m_group_id == group_id);
    CHECK(group.m_name == "default");

    board board = db_manager.select_board(board_id);
    CHECK(board.m_board_id == board_id);
    CHECK(board.m_name == "test_board");
    CHECK(board.m_group_id == group_id);
    CHECK(board.m_description == "test description");

    list list = db_manager.select_list(list_id);
    CHECK(list.m_list_id == list_id);
    CHECK(list.m_board_id == board_id);
    CHECK(list.m_name == "test_list");
    CHECK(list.m_description == "test description");

    card card = db_manager.select_card(card_id);
    CHECK(card.m_card_id == card_id);
    CHECK(card.m_name == "test_card");
    CHECK(card.m_list_id == list_id);
    CHECK(card.m_description == "test description");

    tag tag = db_manager.select_tag(tag_id);
    CHECK(tag.m_tag_id == tag_id);
    CHECK(tag.m_name == "test_tag");
}

#endif

#ifdef TEST_NEW_FEATURE

TEST_CASE("new feature") {
    db_manager db_manager(arguments[0], arguments[1], arguments[2], arguments[3]);
    db_manager.clear_all_tables();
    quint32 user_id = db_manager.authorize_user("doctest_user", "");
    quint32 group_id = db_manager.create_group("doctest_group");
    db_manager.add_user_to_group(user_id, group_id);
    db_manager.insert_board(group_id, "doctest_board", "desc");
}

#endif