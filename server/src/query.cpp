#include "query.hpp"
#include <iostream>

void update_query::execute() {
    std::cout << "Update Query: " << new_value << ' ' << value_name << "\n";
}

void delete_query::execute() {
    std::cout << "Delete Query: " << value_id << ' ' << value_type << "\n";
}

void create_query::execute() {
    std::cout << "Create Query: " << value_type << ' ' << value_name << "\n";
}