#ifndef HASHES_HPP_
#define HASHES_HPP_

#include <QByteArray>
#include <QString>
#include <string>

QString generate_salt();
QString hash_string(const QString &word, const QString &salt);

#endif
