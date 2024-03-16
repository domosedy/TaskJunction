#ifndef CARD_HPP_
#define CARD_HPP_

#include <QObject>
#include <QString>

class Card {
public:
    Card(QString name, QString description, quint16 id = 0);
    Card() = default;

    QString get_name() const;
    QString get_description() const;
    quint16 get_id() const;

protected:
    QString m_name = "New card";
    QString m_description = "New card";
    quint16 m_id = 0;
};

class List {
public:
    List(QString name, QString description, quint16 id = 0);
    List() = default;

    QString get_name() const;
    QString get_description() const;
    quint16 get_id() const;

protected:
    QString m_name = "New list";
    QString m_description = "New list";
    quint16 m_id = 0;
};

class Board {
public:
    Board(QString name, QString description, quint16 id = 0);
    Board() = default;

    QString get_name() const;
    QString get_description() const;
    quint16 get_id() const;

protected:
    QString m_name = "New board";
    QString m_description = "New board";
    quint16 m_id = 0;
};

#endif  // CARD_HPP_