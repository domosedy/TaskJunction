#include "datas.hpp"
#include <sstream>

template<typename T> 
requires has_to_json_method<T>
static std::string array_to_json(const std::vector<T> &data) {
    std::stringstream ss;

    for (std::size_t i = 0; i + 1 < data.size(); ++i) {
        ss << data[i].to_json() << ", ";
    }

    if (!data.empty()) {
        ss << data.back().to_json();
    }

    return ss.str();
}

std::string Tag::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"tag\", \"id\": " << id
         << ", \"name\": \"" << name << "\" }";
    return ss.str();
}

std::string Card::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"card\", \"tags\": [";
    ss << array_to_json(tags);

    ss << "], \"id\": " << id 
       << ", \"name\": \"" << name << "\"" 
       << ", \"description\": \"" << description << "\"}";

    return ss.str();
}

std::string List::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"list\", \"cards\": [";
    ss << array_to_json(cards);

    ss << "], \"id\": " << id 
       << ", \"name\": \"" << name << "\"" 
       << ", \"description\": \"" << description << "\"}";

    return ss.str();
}

std::string Board::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"board\", \"lists\": [";
    ss << array_to_json(lists);

    ss << "], \"id\": " << id 
       << ", \"name\": \"" << name << "\"" 
       << ", \"description\": \"" << description << "\"}";

    return ss.str();
}

std::string ErrorJson::to_json() const {
    std::stringstream ss;
    ss << "{ \"type\": \"error\", \"msg\": \"" << error_message << "\" }";

    return ss.str();
}