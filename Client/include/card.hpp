#ifndef CARD_HPP_
#define CARD_HPP_

#include <QObject>
#include <QString>

class Card {
public:
    Card(QString name = "card", QString description = "random");

    QString get_name() const;
    QString get_description() const;
signals:

private:
    QString name;
    QString description;
};

#endif  // CARD_HPP_