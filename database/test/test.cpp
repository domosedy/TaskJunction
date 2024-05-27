#include <iostream>
#include <sstream>
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
    const int N = 5;
    {
        for (int i = 1; i < N; ++i) {
            quint32 id = db_manager.authorize_user(
                "test_user" + QString::number(i), "test_password"
            );
            CHECK(id == i);
            auto user = db_manager.select_user(id);
            CHECK(user.m_name == "test_user" + QString::number(i));
            CHECK(user.m_user_id == id);
        }
    }
    {
        for (int i = 1; i < N; ++i) {
            quint32 id =
                db_manager.insert_board(1, "test_board", "test description", "");
            CHECK(id == i);
        }
    }
    {
        for (int i = 1; i < N; ++i) {
            quint32 id =
                db_manager.insert_list(1, "test_list", "test description");
            CHECK(id == i);
        }
    }
    {
        for (int i = 1; i < 5; ++i) {
            quint32 id =
                db_manager.insert_card(1, "test_card", "test description");
            CHECK(id == i);
        }
    }
    {
        for (int i = 1; i < N; ++i) {
            quint32 id = db_manager.insert_tag("test_tag" + QString::number(i));
            CHECK(id == i);
        }
    }
}

TEST_CASE("select") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id =
        db_manager.insert_board(user_id, "test_board", "test description", "test link");
    quint32 list_id =
        db_manager.insert_list(board_id, "test_list", "test description");
    quint32 card_id =
        db_manager.insert_card(list_id, "test_card", "test description");
    quint32 tag_id = db_manager.insert_tag("test_tag");

    SUBCASE("select user") {
        user user = db_manager.select_user(user_id);
        CHECK(user.m_user_id == user_id);
        CHECK(user.m_name == "test_user");
    }

    SUBCASE("select board") {
        board board = db_manager.select_board(board_id);
        CHECK(board.m_board_id == board_id);
        CHECK(board.m_name == "test_board");
        CHECK(board.m_user_id == user_id);
        CHECK(board.m_description == "test description");
        CHECK(board.m_link == "test link");
    }

    SUBCASE("select list") {
        list list = db_manager.select_list(list_id);
        CHECK(list.m_list_id == list_id);
        CHECK(list.m_board_id == board_id);
        CHECK(list.m_name == "test_list");
        CHECK(list.m_description == "test description");
    }

    SUBCASE("select card") {
        card card = db_manager.select_card(card_id);
        CHECK(card.m_card_id == card_id);
        CHECK(card.m_name == "test_card");
        CHECK(card.m_list_id == list_id);
        CHECK(card.m_description == "test description");
    }

    SUBCASE("select tag") {
        tag tag = db_manager.select_tag(tag_id);
        CHECK(tag.m_tag_id == tag_id);
        CHECK(tag.m_name == "test_tag");
    }
}

TEST_CASE("delete") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );

    auto create = [&]() {
        db_manager.clear_all_tables();
        quint32 user_id =
            db_manager.authorize_user("test_user", "test_password");
        quint32 board_id = db_manager.insert_board(user_id, "test_board", "", "");
        quint32 list_id = db_manager.insert_list(board_id, "test_list", "");
        quint32 card_id = db_manager.insert_card(list_id, "test_card", "");
        quint32 tag_id = db_manager.insert_tag("test_name");
        db_manager.add_tag_to_card(card_id, tag_id);
        return QVector<quint32>{user_id, board_id, list_id, card_id, tag_id};
    };

    SUBCASE("delete card") {
        auto ids = create();
        db_manager.delete_card(ids[3]);
        auto result = db_manager.get_full_board(ids[1]);
        CHECK(result.m_user_id == ids[0]);
        CHECK(result.m_board_id == ids[1]);
        CHECK(result.m_lists[0].m_list_id == ids[2]);
        CHECK(result.m_lists[0].m_cards.empty());
    }

    SUBCASE("delete list") {
        auto ids = create();
        db_manager.delete_list(ids[2]);
        auto result = db_manager.get_full_board(ids[1]);
        CHECK(result.m_user_id == ids[0]);
        CHECK(result.m_board_id == ids[1]);
        CHECK(result.m_lists.empty());
    }

    SUBCASE("delete board") {
        auto ids = create();
        db_manager.delete_board(ids[1]);
        CHECK(db_manager.get_user_boards(ids[0]).empty());
    }
}

TEST_CASE("get user boards") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id_1 = db_manager.insert_board(user_id, "board_1", "", "");
    quint32 board_id_2 = db_manager.insert_board(user_id, "board_2", "", "");
    quint32 board_id_3 = db_manager.insert_board(user_id, "board_3", "", "");
    QVector<quint32> answer = {board_id_1, board_id_2, board_id_3};

    auto result = db_manager.get_user_boards(user_id);
    CHECK(answer.size() == result.size());
    for (int i = 0; i < answer.size(); ++i) {
        CHECK(answer[i] == result[i].m_board_id);
    }
}

TEST_CASE("update order") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id =
            db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(user_id, "test_board", "", "");
    quint32 list_id = db_manager.insert_list(board_id, "test_list", "");

    SUBCASE("normal update order") {
        quint32 card_id_1 = db_manager.insert_card(list_id, "test_card_1", "");
        quint32 card_id_2 = db_manager.insert_card(list_id, "test_card_2", "");
        CHECK(db_manager.move_card(card_id_1, list_id, 1));
        CHECK(db_manager.get_card_number(card_id_1) == 2);
        CHECK(db_manager.get_card_number(card_id_2) == 1);

        quint32 card_id_3 = db_manager.insert_card(list_id, "test_card_3", "");
        CHECK(db_manager.move_card(card_id_3, list_id, 0));
        CHECK(db_manager.get_card_number(card_id_1) == 3);
        CHECK(db_manager.get_card_number(card_id_2) == 2);
        CHECK(db_manager.get_card_number(card_id_3) == 1);

        CHECK(db_manager.move_card(card_id_3, list_id, 2));
        CHECK(db_manager.get_card_number(card_id_1) == 2);
        CHECK(db_manager.get_card_number(card_id_2) == 1);
        CHECK(db_manager.get_card_number(card_id_3) == 3);
    }

    SUBCASE("prohibited changes") {
        quint32 card_id = db_manager.insert_card(list_id, "test_card", "");
        CHECK(!db_manager.move_card(card_id, list_id, -2));
        CHECK(!db_manager.move_card(card_id, list_id, 100));
    }
}

TEST_CASE("check user rights") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id_1 = db_manager.authorize_user("test_user_1", "");
    quint32 user_id_2 = db_manager.authorize_user("test_user_2", "");
    quint32 board_id_1 = db_manager.insert_board(user_id_1, "board_1", "", "");
    quint32 board_id_2 = db_manager.insert_board(user_id_2, "board_2", "", "");

    CHECK(db_manager.check_user_rights(user_id_1, board_id_1));
    CHECK(db_manager.check_user_rights(user_id_2, board_id_2));

    CHECK(!db_manager.check_user_rights(user_id_1, board_id_2));
    CHECK(!db_manager.check_user_rights(user_id_2, board_id_1));
}

TEST_CASE("update command") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

        quint32 user_id = db_manager.authorize_user("test_user", "test_password");
        quint32 board_id = db_manager.insert_board(user_id, "test_board", "test_description", "test_link");
    SUBCASE("update board") {
        QString new_name = "new_board_name";
        QString new_description = "new_board_description";
        QString new_link = "new_link";

        SUBCASE("normal update") {
            CHECK(db_manager.update_command(
                    BOARD_TABLE_NAME, "name", new_name, board_id
            ));
            CHECK(db_manager.select_board(board_id).m_name == new_name);

            CHECK(db_manager.update_command(
                    BOARD_TABLE_NAME, "description", new_description, board_id
            ));
            CHECK(
                    db_manager.select_board(board_id).m_description == new_description
            );

            CHECK(db_manager.update_command(
                    BOARD_TABLE_NAME, "link", new_link, board_id
            ));
            CHECK(db_manager.select_board(board_id).m_link == new_link);
        }

        SUBCASE("prohibited update") {
            CHECK(!db_manager.update_command(
                    BOARD_TABLE_NAME, "id", "0", board_id
            ));
        }
    }

    quint32 list_id = db_manager.insert_list(board_id, "test_list", "test_description");

    SUBCASE("update list") {
        SUBCASE("normal update") {
            QString new_name = "new_list_name";
            QString new_description = "new_list_description";

            CHECK(db_manager.update_command(
                    LIST_TABLE_NAME, "name", new_name, list_id
            ));
            CHECK(db_manager.select_list(list_id).m_name == new_name);

            CHECK(db_manager.update_command(
                    LIST_TABLE_NAME, "description", new_description, list_id
            ));
            CHECK(
                    db_manager.select_list(list_id).m_description == new_description
            );

        }
#ifdef PROHIBITED_UPDATE_
        SUBCASE("prohibited update") {
            CHECK(!db_manager.update_command(
                    LIST_TABLE_NAME, "id", "0", list_id
            ));
            CHECK(!db_manager.update_command(
                    LIST_TABLE_NAME, "cards_number", "0", list_id
            ));
        }
#endif // PROHIBITED_UPDATE_
    }

    quint32 card_id = db_manager.insert_card(list_id, "test_card", "test_description");

    SUBCASE("update card") {
        SUBCASE("normal update") {
            QString new_name = "new_card_name";
            QString new_description = "new_card_description";

            CHECK(db_manager.update_command(
                    CARD_TABLE_NAME, "name", new_name, card_id
            ));
            CHECK(db_manager.select_card(card_id).m_name == new_name);

            CHECK(db_manager.update_command(
                    CARD_TABLE_NAME, "description", new_description, card_id
            ));
            CHECK(
                    db_manager.select_card(card_id).m_description == new_description
            );
        }
#ifdef PROHIBITED_UPDATE_
        SUBCASE("prohibited update") {
            CHECK(!db_manager.update_command(
                    CARD_TABLE_NAME, "id", "0", card_id
            ));
            CHECK(!db_manager.update_command(
                    CARD_TABLE_NAME, "number", "0", card_id
            ));
        }
#endif  // PROHIBITED_UPDATE_
    }
    quint32 tag_id = db_manager.insert_tag("tag_name");

    SUBCASE("update tag") {
        SUBCASE("normal update") {
            QString new_name = "new_tag_name";

            CHECK(db_manager.update_command(
                    TAG_TABLE_NAME, "name", new_name, tag_id
            ));
            CHECK(db_manager.select_tag(tag_id).m_name == new_name);
        }
#ifdef PROHIBITED_UPDATE_
        SUBCASE("prohibited update") {
            CHECK(!db_manager.update_command(
                    CARD_TABLE_NAME, "id", "0", tag_id
            ));
        }
#endif // PROHIBITED_UPDATE_
    }
}

TEST_CASE("adding a user to a board") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id_1 =
        db_manager.authorize_user("test_user_1", "test_password");
    quint32 user_id_2 =
        db_manager.authorize_user("test_user_2", "test_password");

    quint32 board_id = db_manager.insert_board(user_id_1, "test_board", "", "");

    SUBCASE("normal adding") {
        CHECK(db_manager.add_user_to_board(user_id_2, board_id));

        QVector<quint32> answer = {user_id_1, user_id_2};
        CHECK(db_manager.get_board_user_ids(board_id) == answer);
    }

    SUBCASE("non-existent board or user") {
        CHECK(!db_manager.add_user_to_board(user_id_1, 0));
        CHECK(!db_manager.add_user_to_board(0, board_id));

        CHECK(!db_manager.add_user_to_board(user_id_1, 100));
        CHECK(!db_manager.add_user_to_board(100, board_id));
    }
}

TEST_CASE("delete user from board") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    QVector<quint32> user_ids = {
        db_manager.authorize_user("test_user_1", "test_password"),
        db_manager.authorize_user("test_user_2", "test_password"),
        db_manager.authorize_user("test_user_3", "test_password")
    };
    quint32 board_id = db_manager.insert_board(user_ids[0], "test_board", "", "");

    CHECK(db_manager.add_user_to_board(user_ids[1], board_id));
    CHECK(db_manager.add_user_to_board(user_ids[2], board_id));

    SUBCASE("normal deletion") {
        db_manager.delete_user_from_board(user_ids[2], board_id);

        auto answer = {user_ids[0], user_ids[1]};
        CHECK(db_manager.get_board_user_ids(board_id) == answer);
    }

    SUBCASE("non-existent board or user") {

        CHECK(!db_manager.delete_user_from_board(user_ids[0], 0));
        CHECK(!db_manager.delete_user_from_board(0, board_id));

        CHECK(!db_manager.delete_user_from_board(user_ids[0], 100));
        CHECK(!db_manager.delete_user_from_board(100, board_id));
    }
}

TEST_CASE("get board card ids") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();
    auto user_id = db_manager.authorize_user("test_user", "test_password");
    auto board_id = db_manager.insert_board(user_id, "", "", "");
    auto list_id_1 = db_manager.insert_list(board_id, "", "");
    auto list_id_2 = db_manager.insert_list(board_id, "", "");
    db_manager.insert_card(list_id_1, "", "");
    db_manager.insert_card(list_id_1, "", "");
    db_manager.insert_card(list_id_2, "", "");
    db_manager.insert_card(list_id_2, "", "");

    auto card_ids = db_manager.get_board_card_ids(board_id);
    QVector<quint32> answer = {1, 3, 2, 4};
    CHECK(card_ids == answer);
}

TEST_CASE("cards filter") {
    db_manager db_manager(
            arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();
    auto user_id = db_manager.authorize_user("test_user", "test_password");
    auto board_id_1 = db_manager.insert_board(user_id, "", "", "");
    auto board_id_2 = db_manager.insert_board(user_id, "", "", "");
    auto list_id_1 = db_manager.insert_list(board_id_1, "", "");
    auto list_id_2 = db_manager.insert_list(board_id_2, "", "");
    auto card_id_1 = db_manager.insert_card(list_id_1, "", "");
    auto card_id_2 = db_manager.insert_card(list_id_1, "", "");
    auto card_id_3 = db_manager.insert_card(list_id_1, "", "");
    auto card_id_4 = db_manager.insert_card(list_id_2, "", "");
    auto card_id_5 = db_manager.insert_card(list_id_2, "", "");
    auto card_id_6 = db_manager.insert_card(list_id_2, "", "");
    auto tag_id_1 = db_manager.insert_tag("1");
    auto tag_id_2 = db_manager.insert_tag("2");
    auto tag_id_3 = db_manager.insert_tag("3");
    db_manager.add_tag_to_card(card_id_1, tag_id_1);
    db_manager.add_tag_to_card(card_id_2, tag_id_2);
    db_manager.add_tag_to_card(card_id_3, tag_id_3);
    db_manager.add_tag_to_card(card_id_4, tag_id_1);
    db_manager.add_tag_to_card(card_id_5, tag_id_2);
    db_manager.add_tag_to_card(card_id_6, tag_id_3);

    QSet<quint32> answer = {card_id_1, card_id_2};
    QVector<QString> array = {"1", "2"};
    CHECK(db_manager.filter_cards(board_id_1, array) == answer);
}

TEST_CASE("get board_id by link") {
    db_manager db_manager(
            arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");

    quint32 board_id_1 =
            db_manager.insert_board(user_id, "board 1", "", "link_1");
    quint32 board_id_2 =
            db_manager.insert_board(user_id, "board 2", "", "link_2");
    quint32 board_id_3 =
            db_manager.insert_board(user_id, "board 3", "", "link_3");

    CHECK(db_manager.get_board_by_link("link_1") == board_id_1);
    CHECK(db_manager.get_board_by_link("link_2") == board_id_2);
    CHECK(db_manager.get_board_by_link("link_3") == board_id_3);
}

// TODO trouble with updating order, id, etc fields (must be banned)

#endif

#ifdef TEST_NEW_FEATURE

TEST_CASE("new feature") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id =
            db_manager.insert_board(user_id, "test_board", "test description", "test link");
    quint32 list_id_1 =
            db_manager.insert_list(board_id, "test_list_1", "test description");
    quint32 list_id_2 =
            db_manager.insert_list(board_id, "test_list_2", "test description");
    quint32 list_id_3 =
            db_manager.insert_list(board_id, "test_list_3", "test description");

    db_manager.update_command(LIST_TABLE_NAME, "name", "new_name", list_id_3);

    {
        auto result = db_manager.get_board_lists(board_id);
        CHECK(result[0].m_list_id == list_id_1);
        CHECK(result[1].m_list_id == list_id_2);
        CHECK(result[2].m_list_id == list_id_3);
    }

    {
        auto result = db_manager.get_board_list_ids(board_id);
        CHECK(result[0] == list_id_1);
        CHECK(result[1] == list_id_2);
        CHECK(result[2] == list_id_3);
    }
}

#endif
