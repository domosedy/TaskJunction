#include <sstream>
#include "database.hpp"
#include "doctest.h"
#include "element_classes.hpp"
#include "test_utils.hpp"

using namespace database;
QVector<QString> arguments = {"postgres", "ivan", "localhost", "1"};

#ifdef DEFAULT_TESTS

TEST_CASE("database is open") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );

    CHECK(db_manager.is_open());
}

TEST_CASE("create") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();
    const int N = 5;
    for (int i = 1; i < N; ++i) {
        quint32 id = db_manager.authorize_user(
            "test_user" + QString::number(i), "test_password"
        );
        CHECK(id == i);
        auto user = db_manager.select_user(id);
        CHECK(user.m_name == "test_user" + QString::number(i));
        CHECK(user.m_user_id == id);
    }
    for (int i = 1; i < N; ++i) {
        quint32 id = db_manager.insert_board(
            1, "test_board", "test description", ""
        );
        CHECK(id == i);
    }
    for (int i = 1; i < N; ++i) {
        quint32 id =
            db_manager.insert_list(1, "test_list", "test description");
        CHECK(id == i);
    }
    for (int i = 1; i < 5; ++i) {
        quint32 id =
            db_manager.insert_card(1, "test_card", "test description");
        CHECK(id == i);
    }
    for (int i = 1; i < N; ++i) {
        quint32 id = db_manager.insert_tag("test_tag" + QString::number(i));
        CHECK(id == i);
    }
}

TEST_CASE("select") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(
        user_id, "test_board", "test description", "test link"
    );
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

    db_manager.clear_all_tables();
    quint32 user_id =
        db_manager.authorize_user("test_user", "test_password");
    quint32 board_id =
        db_manager.insert_board(user_id, "test_board", "", "");
    quint32 list_id = db_manager.insert_list(board_id, "test_list", "");
    quint32 card_id = db_manager.insert_card(list_id, "test_card", "");
    quint32 tag_id = db_manager.insert_tag("test_name");
    db_manager.add_tag_to_card(card_id, tag_id);

    SUBCASE("delete user") {
        CHECK(db_manager.delete_user(user_id));
        CHECK(db_manager.get_user_boards(user_id).empty());
    }

    SUBCASE("delete board") {
        CHECK(db_manager.delete_board(board_id));
        CHECK(db_manager.get_user_boards(user_id).empty());
    }

    SUBCASE("delete list") {
        CHECK(db_manager.delete_list(list_id));
        auto result = db_manager.get_full_board(board_id);
        CHECK(result.m_user_id == user_id);
        CHECK(result.m_board_id == board_id);
        CHECK(result.m_lists.empty());
    }

    SUBCASE("delete card") {
        CHECK(db_manager.delete_card(card_id));
        auto result = db_manager.get_full_board(board_id);
        CHECK(result.m_user_id == user_id);
        CHECK(result.m_board_id == board_id);
        CHECK(result.m_lists[0].m_list_id == list_id);
        CHECK(result.m_lists[0].m_cards.empty());
    }

    SUBCASE("delete tag") {
        CHECK(db_manager.delete_tag(tag_id));
        CHECK(db_manager.get_card_tags(card_id).empty());
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

TEST_CASE("get board lists") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(user_id, "test_board", "", "");
    quint32 list_id_1 = db_manager.insert_list(board_id, "test_list_1", "");
    quint32 list_id_2 = db_manager.insert_list(board_id, "test_list_2", "");
    quint32 list_id_3 = db_manager.insert_list(board_id, "test_list_2", "");
    QVector<quint32> answer = {list_id_1, list_id_2, list_id_3};

    SUBCASE("get board lists") {
        auto result = db_manager.get_board_lists(board_id);
        CHECK(answer.size() == result.size());
        for (int i = 0; i < answer.size(); ++i) {
            CHECK(answer[i] == result[i].m_list_id);
        }
    }
    SUBCASE("get board list ids") {
        CHECK(answer == db_manager.get_board_list_ids(board_id));
    }
}

TEST_CASE("get list cards") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(user_id, "test_board", "", "");
    quint32 list_id = db_manager.insert_list(board_id, "test_list", "");
    quint32 card_id_1 = db_manager.insert_card(list_id, "test_card_1", "");
    quint32 card_id_2 = db_manager.insert_card(list_id, "test_card_2", "");
    quint32 card_id_3 = db_manager.insert_card(list_id, "test_card_2", "");
    QVector<quint32> answer = {card_id_1, card_id_2, card_id_3};

    SUBCASE("get list cards") {
        auto result = db_manager.get_list_cards(list_id);
        CHECK(answer.size() == result.size());
        for (int i = 0; i < answer.size(); ++i) {
            CHECK(answer[i] == result[i].m_card_id);
        }
    }
    SUBCASE("get list card ids") {
        CHECK(answer == db_manager.get_list_card_ids(list_id));
    }
}

TEST_CASE("get card tags") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(user_id, "test_board", "", "");
    quint32 list_id = db_manager.insert_list(board_id, "test_list", "");
    quint32 card_id = db_manager.insert_card(list_id, "test_card", "");
    quint32 tag_id_1 = db_manager.insert_tag("test_tag_1");
    quint32 tag_id_2 = db_manager.insert_tag("test_tag_2");
    quint32 tag_id_3 = db_manager.insert_tag("test_tag_3");

    CHECK(db_manager.add_tag_to_card(card_id, tag_id_1));
    CHECK(db_manager.add_tag_to_card(card_id, tag_id_2));
    CHECK(db_manager.add_tag_to_card(card_id, tag_id_3));

    QVector<quint32> answer = {tag_id_1, tag_id_2, tag_id_3};

    auto result = db_manager.get_card_tags(card_id);
    CHECK(answer.size() == result.size());
    for (int i = 0; i < answer.size(); ++i) {
        CHECK(answer[i] == result[i].m_tag_id);
    }
}

TEST_CASE("get card number") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(user_id, "test_board", "", "");
    quint32 list_id = db_manager.insert_list(board_id, "test_list", "");
    quint32 card_id_1 = db_manager.insert_card(list_id, "test_card_1", "");
    quint32 card_id_2 = db_manager.insert_card(list_id, "test_card_2", "");
    quint32 card_id_3 = db_manager.insert_card(list_id, "test_card_3", "");

    CHECK(db_manager.get_card_number(card_id_1) == 1);
    CHECK(db_manager.get_card_number(card_id_2) == 2);
    CHECK(db_manager.get_card_number(card_id_3) == 3);
}

TEST_CASE("update order") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
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

    SUBCASE("prohibited update") {
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
    quint32 board_id = db_manager.insert_board(
        user_id, "test_board", "test_description", "test_link"
    );
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
                db_manager.select_board(board_id).m_description ==
                new_description
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

    quint32 list_id =
        db_manager.insert_list(board_id, "test_list", "test_description");

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

        SUBCASE("prohibited update") {
            CHECK(
                !db_manager.update_command(LIST_TABLE_NAME, "id", "0", list_id)
            );
            CHECK(!db_manager.update_command(
                LIST_TABLE_NAME, "cards_number", "0", list_id
            ));
        }
    }

    quint32 card_id =
        db_manager.insert_card(list_id, "test_card", "test_description");

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

        SUBCASE("prohibited update") {
            CHECK(
                !db_manager.update_command(CARD_TABLE_NAME, "id", "0", card_id)
            );
            CHECK(!db_manager.update_command(
                CARD_TABLE_NAME, "number", "0", card_id
            ));
        }
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

        SUBCASE("prohibited update") {
            CHECK(!db_manager.update_command(CARD_TABLE_NAME, "id", "0", tag_id)
            );
        }
    }
}

TEST_CASE("add user to board") {
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
        db_manager.authorize_user("test_user_3", "test_password")};
    quint32 board_id =
        db_manager.insert_board(user_ids[0], "test_board", "", "");

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

TEST_CASE("add tag to card") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id =
        db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(user_id, "test_board", "", "");
    quint32 list_id = db_manager.insert_list(board_id, "test_list", "");
    quint32 card_id_1 = db_manager.insert_card(list_id, "test_card_1", "");
    quint32 card_id_2 = db_manager.insert_card(list_id, "test_card_2", "");
    quint32 tag_id_1 = db_manager.insert_tag("test_tag_1");
    quint32 tag_id_2 = db_manager.insert_tag("test_tag_2");
    SUBCASE("normal adding") {
        CHECK(db_manager.add_tag_to_card(card_id_1, tag_id_1));
        CHECK(db_manager.add_tag_to_card(card_id_2, tag_id_2));

        CHECK(!db_manager.add_tag_to_card(card_id_1, tag_id_1));
        CHECK(!db_manager.add_tag_to_card(card_id_2, tag_id_2));

        auto result_1 = db_manager.get_card_tags(card_id_1);
        auto result_2 = db_manager.get_card_tags(card_id_2);

        CHECK((result_1.size() == 1 && result_1[0].m_tag_id == tag_id_1));
        CHECK((result_2.size() == 1 && result_2[0].m_tag_id == tag_id_2));
    }

    SUBCASE("non-existent board or user") {
        CHECK(!db_manager.add_tag_to_card(card_id_1, 0));
        CHECK(!db_manager.add_tag_to_card(0, card_id_1));

        CHECK(!db_manager.add_tag_to_card(card_id_1, 100));
        CHECK(!db_manager.add_tag_to_card(100, card_id_1));
    }
}

TEST_CASE("delete tag from card") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id =
        db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(user_id, "test_board", "", "");
    quint32 list_id = db_manager.insert_list(board_id, "test_list", "");
    quint32 card_id = db_manager.insert_card(list_id, "test_card", "");

    QVector<quint32> tag_ids = {
        db_manager.insert_tag("test_tag_1"),
        db_manager.insert_tag("test_tag_2"),
        db_manager.insert_tag("test_tag_3")
    };

    CHECK(db_manager.add_tag_to_card(card_id, tag_ids[0]));
    CHECK(db_manager.add_tag_to_card(card_id, tag_ids[1]));
    CHECK(db_manager.add_tag_to_card(card_id, tag_ids[2]));

    SUBCASE("normal deletion") {
        db_manager.delete_tag_from_card(card_id, tag_ids[2]);

        auto result = db_manager.get_card_tags(board_id);
        CHECK(result.size() == 2);
        CHECK(result[0].m_tag_id == tag_ids[0]);
        CHECK(result[1].m_tag_id == tag_ids[1]);
    }

    SUBCASE("non-existent board or user") {
        CHECK(!db_manager.delete_tag_from_card(card_id, 0));
        CHECK(!db_manager.delete_tag_from_card(0, tag_ids[0]));

        CHECK(!db_manager.delete_tag_from_card(card_id, 100));
        CHECK(!db_manager.delete_tag_from_card(100, tag_ids[0]));
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

TEST_CASE("any_filter") {
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
    CHECK(db_manager.any_filter_cards(board_id_1, array) == answer);
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

    CHECK(db_manager.get_board_id_by_link("link_1") == board_id_1);
    CHECK(db_manager.get_board_id_by_link("link_2") == board_id_2);
    CHECK(db_manager.get_board_id_by_link("link_3") == board_id_3);
}

TEST_CASE("all_filter") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    auto user_id = db_manager.authorize_user("test_user", "test_password");
    auto board_id = db_manager.insert_board(user_id, "test_board", "", "");
    auto list_id_1 = db_manager.insert_list(board_id, "test_list_1", "");
    auto list_id_2 = db_manager.insert_list(board_id, "test_list_2", "");
    auto card_id_1 = db_manager.insert_card(list_id_1, "test_card_1", "");
    auto card_id_2 = db_manager.insert_card(list_id_1, "test_card_2", "");
    auto card_id_3 = db_manager.insert_card(list_id_1, "test_card_3", "");
    auto card_id_4 = db_manager.insert_card(list_id_1, "test_card_4", "");
    auto card_id_5 = db_manager.insert_card(list_id_2, "test_card_5", "");
    auto card_id_6 = db_manager.insert_card(list_id_2, "test_card_6", "");
    auto card_id_7 = db_manager.insert_card(list_id_2, "test_card_7", "");
    auto card_id_8 = db_manager.insert_card(list_id_2, "test_card_8", "");
    auto tag_id_1 = db_manager.insert_tag("1");
    auto tag_id_2 = db_manager.insert_tag("2");
    auto tag_id_3 = db_manager.insert_tag("3");
    db_manager.add_tag_to_card(card_id_1, tag_id_1);
    db_manager.add_tag_to_card(card_id_2, tag_id_2);
    db_manager.add_tag_to_card(card_id_3, tag_id_3);
    db_manager.add_tag_to_card(card_id_4, tag_id_1);
    db_manager.add_tag_to_card(card_id_4, tag_id_2);
    db_manager.add_tag_to_card(card_id_5, tag_id_1);
    db_manager.add_tag_to_card(card_id_5, tag_id_3);
    db_manager.add_tag_to_card(card_id_6, tag_id_2);
    db_manager.add_tag_to_card(card_id_6, tag_id_3);
    db_manager.add_tag_to_card(card_id_7, tag_id_1);
    db_manager.add_tag_to_card(card_id_7, tag_id_2);
    db_manager.add_tag_to_card(card_id_7, tag_id_3);

    SUBCASE("zero tags") {
        QVector<QString> array = {};
        auto result = db_manager.all_filter_cards(board_id, array);
        CHECK(result.empty());
    }

    SUBCASE("one tag") {
        QVector<QString> array = {"1"};
        QSet<quint32> answer = {card_id_1, card_id_4, card_id_5, card_id_7};
        auto result = db_manager.all_filter_cards(board_id, array);
        CHECK(result == answer);
    }

    SUBCASE("two tags") {
        QVector<QString> array = {"1", "2"};
        QSet<quint32> answer = {card_id_4, card_id_7};
        auto result = db_manager.all_filter_cards(board_id, array);
        CHECK(result == answer);
    }

    SUBCASE("three tags") {
        QVector<QString> array = {"1", "2", "3"};
        QSet<quint32> answer = {card_id_7};
        auto result = db_manager.all_filter_cards(board_id, array);
        CHECK(result == answer);
    }

    SUBCASE("too many tags") {
        QVector<QString> array = {"1", "2", "3", "4"};
        auto result = db_manager.all_filter_cards(board_id, array);
        CHECK(result.empty());
    }
}

TEST_CASE("get full board") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(
        user_id, "test_board", "test description", "test link"
    );
    quint32 list_id =
        db_manager.insert_list(board_id, "test_list", "test description");

    SUBCASE("simple board") {
        quint32 card_id =
            db_manager.insert_card(list_id, "test_card", "test description");
        quint32 tag_id = db_manager.insert_tag("test_tag");
        db_manager.add_tag_to_card(card_id, tag_id);

        board board = db_manager.get_full_board(board_id);

        CHECK(board.m_user_id == user_id);
        CHECK(board.m_name == "test_board");
        CHECK(board.m_description == "test description");
        CHECK(board.m_link == "test link");

        CHECK(board.m_lists.size() == 1);
        CHECK(board.m_lists[0].m_board_id == board_id);
        CHECK(board.m_lists[0].m_name == "test_list");
        CHECK(board.m_lists[0].m_description == "test description");

        CHECK(board.m_lists[0].m_cards.size() == 1);
        CHECK(board.m_lists[0].m_cards[0].m_list_id == list_id);
        CHECK(board.m_lists[0].m_cards[0].m_name == "test_card");
        CHECK(board.m_lists[0].m_cards[0].m_description == "test description");

        CHECK(board.m_lists[0].m_cards[0].m_tags.size() == 1);
        CHECK(board.m_lists[0].m_cards[0].m_tags[0].m_tag_id == tag_id);
        CHECK(board.m_lists[0].m_cards[0].m_tags[0].m_name == "test_tag");
    }

    SUBCASE("check cards order") {
        quint32 card_id_1 = db_manager.insert_card(list_id, "test_card_1", "test description");
        quint32 card_id_2 = db_manager.insert_card(list_id, "test_card_2", "test description");
        quint32 card_id_3 = db_manager.insert_card(list_id, "test_card_3", "test description");

        board board = db_manager.get_full_board(board_id);

        CHECK(board.m_lists.size() == 1);
        CHECK(board.m_lists[0].m_cards.size() == 3);
        CHECK(board.m_lists[0].m_cards[0].m_card_id == card_id_1);
        CHECK(board.m_lists[0].m_cards[1].m_card_id == card_id_2);
        CHECK(board.m_lists[0].m_cards[2].m_card_id == card_id_3);
    }
}

TEST_CASE("copy board") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(
        user_id, "test_board", "test description", "test link"
    );
    quint32 list_id =
        db_manager.insert_list(board_id, "test_list", "test description");

    SUBCASE("simple board") {
        quint32 card_id =
            db_manager.insert_card(list_id, "test_card", "test description");
        quint32 tag_id = db_manager.insert_tag("test_tag");
        db_manager.add_tag_to_card(card_id, tag_id);

        board board =
            db_manager.copy_board(db_manager.get_full_board(board_id), user_id);

        CHECK(board.m_user_id == user_id);
        CHECK(board.m_name == "test_board");
        CHECK(board.m_description == "test description");
        CHECK(board.m_link == "test link");

        CHECK(board.m_lists.size() == 1);
        CHECK(board.m_lists[0].m_board_id == board.m_board_id);
        CHECK(board.m_lists[0].m_name == "test_list");
        CHECK(board.m_lists[0].m_description == "test description");

        CHECK(board.m_lists[0].m_cards.size() == 1);
        CHECK(
            board.m_lists[0].m_cards[0].m_list_id == board.m_lists[0].m_list_id
        );
        CHECK(board.m_lists[0].m_cards[0].m_name == "test_card");
        CHECK(board.m_lists[0].m_cards[0].m_description == "test description");

        CHECK(board.m_lists[0].m_cards[0].m_tags.size() == 1);
        CHECK(board.m_lists[0].m_cards[0].m_tags[0].m_tag_id == tag_id);
        CHECK(board.m_lists[0].m_cards[0].m_tags[0].m_name == "test_tag");
    }

    SUBCASE("check cards order") {
        quint32 card_id_1 = db_manager.insert_card(list_id, "test_card_1", "test description");
        quint32 card_id_2 = db_manager.insert_card(list_id, "test_card_2", "test description");
        quint32 card_id_3 = db_manager.insert_card(list_id, "test_card_3", "test description");

        board board =
            db_manager.copy_board(db_manager.get_full_board(board_id), user_id);

        CHECK(board.m_lists.size() == 1);
        CHECK(board.m_lists[0].m_cards.size() == 3);
        CHECK(board.m_lists[0].m_cards[0].m_name == "test_card_1");
        CHECK(board.m_lists[0].m_cards[1].m_name == "test_card_2");
        CHECK(board.m_lists[0].m_cards[2].m_name == "test_card_3");
    }
}

#endif  // DEFAULT_TESTS

#ifdef TEST_NEW_FEATURE

TEST_CASE("new feature") {
    db_manager db_manager(
        arguments[0], arguments[1], arguments[2], arguments[3]
    );
    db_manager.clear_all_tables();

    quint32 user_id = db_manager.authorize_user("test_user", "test_password");
    quint32 board_id = db_manager.insert_board(
        user_id, "test_board", "test description", "test link"
    );
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

#endif  // TEST_NEW_FEATURE
