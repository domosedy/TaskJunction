#ifndef HASHES_HPP_
#define HASHES_HPP_

#include <string>
#include <QByteArray>
#include <QString>

QString generate_salt();
QString hash_string(const QString &word, const QString &salt);


#endif