#include <iostream>
#include "datas.hpp"

int main() {
    uint id = 1;
    Tag t1{id, "Tag 1"};
    Tag t2{++id, "Tag 2"};

    Card c1{++id, std::vector{t1}, "Card 1", "Descr 1"};
    Card c2{++id, std::vector{t2}, "Card 1", "Descr 2"};
    Card c3{++id, std::vector{t1, t2}, "Card 1", "Descr 3"};
    Card c4{++id, {}, "Card 1", "Descr 1"};

    List l1{++id, std::vector{c1}, "List 1", "Descr 1"};
    List l2{++id, std::vector{c1, c2}, "List 1", "Descr 1"};
    List l3{++id, std::vector{c1, c3}, "List 1", "Descr 1"};
    List l4{++id, {}, "List 1", "Descr 1"};

    Board b1{++id, std::vector{l1}, "Board 1", "Descr 1"};
    Board b2{++id, std::vector{l1, l2}, "Board 1", "Descr 1"};
    Board b3{++id, std::vector{l1, l3}, "Board 1", "Descr 1"};
    Board b4{++id, {}, "Board 1", "Descr 1"};

    LoginResponse log;
    log.authorized = 1;
    log.all_boards = {
        std::pair{0, "12"}, std::pair{1, "13"}, std::pair{2, "hello"}};

    std::cout << log.to_json() << std::endl;
    std::cout << b1.to_json() << std::endl;
    std::cout << b2.to_json() << std::endl;
    std::cout << b3.to_json() << std::endl;
    std::cout << b4.to_json() << std::endl;
}