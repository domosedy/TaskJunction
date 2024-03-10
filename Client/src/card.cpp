#include "card.hpp"
#include <QDebug>
#include <QObject>

Card::Card(QString name, QString description)
    : name(std::move(name)), description(std::move(description)) {
}

QString Card::get_name() const {
    return name;
}

QString Card::get_description() const {
    return description;
}