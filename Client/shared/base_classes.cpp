#include "base_classes.hpp"
#include <QDebug>
#include <QObject>

Card::Card(QString name, QString description, quint16 id)
    : m_name(std::move(name)), m_description(std::move(description)), m_id(id) {
}

QString Card::get_name() const {
    return m_name;
}

QString Card::get_description() const {
    return m_description;
}

quint16 Card::get_id() const {
    return m_id;
}

List::List(QString name, QString description, quint16 id)
    : m_name(std::move(name)), m_description(std::move(description)), m_id(id) {
}

QString List::get_name() const {
    return m_name;
}

QString List::get_description() const {
    return m_description;
}

quint16 List::get_id() const {
    return m_id;
}

Board::Board(QString name, QString description, quint16 id)
    : m_name(std::move(name)), m_description(std::move(description)), m_id(id) {
}

QString Board::get_name() const {
    return m_name;
}

QString Board::get_description() const {
    return m_description;
}

quint16 Board::get_id() const {
    return m_id;
}
