#ifndef BASIC_HPP_
#define BASIC_HPP_

struct Card_Base {
    Card_Base(QString name = "card", QString description = "random");
    QString m_name;
    QString m_description;
    uint m_card_id;
    uint m_list_id;
};

struct List_Base {
    List_Base(QString name = "card", QString description = "random");
    QString m_name;
    QString m_description;
    uint m_list_id;
    uint m_board_id;
};

struct Board_Base {
    Board_Base(QString name, QString description);
    QString m_name;
    QString m_description;
    uint m_board_id;
};

#endif