#ifndef CARD_HPP_
#define CARD_HPP_

#include <QObject>
#include <QString>

class Card {

public:
    Card() = default;
    Card(QString name = "card", QString description = "");

    QString get_name() const;
    QString get_description() const;

private:
    QString name;
    QString description;
};

#endif  // CARD_HPP_