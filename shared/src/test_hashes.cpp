#include "hashes.hpp"
#include <iostream>

int main() {
    std::cout << generate_salt().toStdString() << std::endl;
    std::cout << hash_string("qwerty12345", generate_salt()).toStdString() << std::endl;
}