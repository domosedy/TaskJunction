#include "element_classes.hpp"
#include <iostream>


int main() {
    uint id = 1;
    tag t1;
    t1.m_tag_id = id++;
    t1.m_name = "Tag 1";

    tag t2;
    t2.m_tag_id = id++;
    t2.m_name = "Tag 2";


    card c1;
    c1.m_card_id = id++;
    c1.m_tags = QVector{t1, t2};
    c1.m_name = "Card 1";
    c1.m_description = "Descr 1";


    list l1;
    l1.m_list_id = id++;
    l1.m_cards = QVector{c1};
    l1.m_name = "List 1";
    l1.m_description = "Descr 2";

    board b1;
    b1.m_board_id = id++;
    b1.m_name = "Board 1";
    b1.m_lists = QVector{l1};

    std::cout << b1.to_json() << std::endl;

    login log;
    log.m_response = 1;
    log.m_boards;
    log.m_boards.push_back(std::pair{0, QString("12")});
    log.m_boards.push_back(std::pair{1, QString("13")});
    log.m_boards.push_back(std::pair{2, QString("hello")});

    std::cout << log.to_json() << std::endl;
    // LoginResponse log;
    // log.authorized = 1;

    // std::cout << log.to_json() << std::endl;
    // std::cout << b1.to_json() << std::endl;
    // std::cout << b2.to_json() << std::endl;
    // std::cout << b3.to_json() << std::endl;
    // std::cout << b4.to_json() << std::endl;
}