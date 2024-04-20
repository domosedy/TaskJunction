#include "hashes.hpp"
#include <QCryptographicHash>
#include <chrono>
#include <random>

QString generate_salt() {
    std::mt19937 rnd(std::chrono::system_clock::now().time_since_epoch().count()
    );

    QByteArray byte_data;

    for (int i = 0; i < 10; ++i) {
        auto character = static_cast<unsigned char>(rnd());
        while (character == '\n') {
            character = static_cast<unsigned char>(rnd());
        }

        byte_data.push_back(character);
    }

    return byte_data.toStdString().c_str();
}

QString hash_string(const QString &word, const QString &salt) {
    QCryptographicHash hash_algo(QCryptographicHash::Algorithm::Sha3_256);
    hash_algo.addData(word.toUtf8() + salt.toUtf8());

    return hash_algo.result().toStdString().c_str();
}
