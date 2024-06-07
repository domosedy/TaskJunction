#ifndef HASHES_HPP_
#define HASHES_HPP_

#include <QByteArray>
#include <QString>
#include <string>
#include <utility>

QString code_string(const QString &link, quint32 id);
std::pair<QString, quint32> decode_string(const QString &link);
QString generate_string();
QString hash_string(const QString &word, const QString &salt);

#endif
