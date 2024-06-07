#include "hashes.hpp"
#include <QCryptographicHash>
#include <chrono>
#include <iostream>
#include <random>

QString code_string(const QString &link, quint32 id) {
  std::mt19937 rnd(std::chrono::system_clock::now().time_since_epoch().count());
  QString coded_link = "";
  QString number = QString::number(id);

  quint32 link_pos = 0;
  quint32 number_pos = 0;
  auto summary_size = link.size() + number.size();

  while (link_pos < link.size() || number_pos < number.size()) {
    if (link_pos == link.size()) {
      coded_link += number[number_pos++];
    } else if (number_pos == number.size()) {
      coded_link += link[link_pos++];
    } else if (static_cast<quint32>(rnd() % summary_size) < link.size()) {
      coded_link += link[link_pos++];
    } else {
      coded_link += number[number_pos++];
    }
  }

  return coded_link;
}

std::pair<QString, quint32> decode_string(const QString &coded_link) {
  QString link = "";
  quint32 id = 0;

  for (auto &it : coded_link) {
    if (it.isNumber()) {
      // QDebug() << it;
      id = id * 10 + it.digitValue();
      std::cout << id << std::endl;
    } else {
      link += it;
    }
  }

  return {link, id};
}

QString generate_string() {
  std::mt19937 rnd(std::chrono::system_clock::now().time_since_epoch().count());

  static const QString all_characters =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!#$%&()*+-/"
      ":;<=>?@";
  QString data;

  for (int i = 0; i < 15; ++i) {
    data += all_characters[rnd() % all_characters.size()];
  }

  return data;
}

QString hash_string(const QString &word, const QString &salt) {
  QCryptographicHash hash_algo(QCryptographicHash::Algorithm::Sha3_256);
  hash_algo.addData(word.toUtf8() + salt.toUtf8());

  return hash_algo.result().toStdString().c_str();
}
