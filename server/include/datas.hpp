#ifndef DATAS_HPP_
#define DATAS_HPP_

#include <vector>
#include <string>
#include <concepts>

template<typename T>
concept has_to_json_method =
requires (T t) { 
    t.to_json(); 
};

struct Tag {
    uint id;
    std::string name;

    std::string to_json() const;
};

struct Card {
    uint id;
    std::vector<Tag> tags;
    std::string name;
    std::string description;

public:
    std::string to_json() const;
};

struct List {
    uint id;
    std::vector<Card> cards;
    std::string name;
    std::string description;

    std::string to_json() const;
};

struct Board {
    uint id;
    std::vector<List> lists;
    std::string name;
    std::string description;

    std::string to_json() const;
};

struct ErrorJson {
    std::string error_message;

    std::string to_json() const;
};

struct AllBoardsJson {
    std::vector<uint> all_boards;

    std::string to_json() const;
};

#endif