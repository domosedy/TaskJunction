#include <iostream>
#include "hashes.hpp"

int main() {
    QString lnk = generate_string();
    quint32 id = 123;

    auto hashed_string = code_string(lnk, id);
    auto val = decode_string("OqDI7H@R#CshY1/n");

    std::cout << hashed_string.toStdString() << std::endl;
    std::cout << val.first.toStdString() << ' ' << val.second << std::endl;
    std::cout << generate_string().toStdString() << std::endl;
    std::cout << hash_string("qwerty12345", generate_string()).toStdString()
              << std::endl;
}