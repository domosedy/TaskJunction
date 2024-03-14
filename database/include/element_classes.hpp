#ifndef ELEMENT_CLASSES_HPP_
#define ELEMENT_CLASSES_HPP_
#include <QtCore>

class tag {
    unsigned m_tag_id;
    QString m_name;

public:
    explicit tag(QVector<QVariant> data);
};

class card {
    unsigned m_card_id;
    unsigned m_list_id;
    QString m_name;
    QString m_description;
    QVector<tag> m_tags;

public:
    explicit card(QVector<QVariant> data);
};

class list {
    unsigned m_list_id;
    unsigned m_board_id;
    QString m_name;
    QString m_description;
    QVector<card> m_cards;

public:
    explicit list(QVector<QVariant> data);

    void print_data() const {
        qDebug() << m_list_id << m_board_id << m_name << m_description;
    }
};

class board {
    unsigned m_board_id;
    unsigned m_user_id;
    QString m_name;
    QVector<list> m_lists;

public:
    explicit board(QVector<QVariant> data);

    void print_data() const {
        qDebug() << m_board_id << ' ' << m_name;
    }
};

//
// struct abstract_object {
//    virtual void get_subobjects() = 0;
//};
//
// class card_with_methods : card, abstract_object {
//    void get_subobjects() override;
//};
//
// class list_with_methods : list, abstract_object {
//    void get_subobjects() override;
//};
//
// class board_with_methods : board, abstract_object {
//    void get_subobjects() override;
//};

board::board(QVector<QVariant> data)
    : m_board_id(data[0].toInt()),
      m_user_id(data[1].toInt()),
      m_name(data[2].toString()) {
}

list::list(QVector<QVariant> data)
    : m_list_id(data[0].toInt()),
      m_board_id(data[1].toInt()),
      m_name(data[2].toString()),
      m_description(data[3].toString()) {
}

card::card(QVector<QVariant> data)
    : m_card_id(data[0].toInt()),
      m_list_id(data[1].toInt()),
      m_name(data[2].toString()),
      m_description(data[3].toString()) {
}

tag::tag(QVector<QVariant> data)
    : m_tag_id(data[0].toInt()), m_name(data[1].toString()) {
}

#endif  // ELEMENT_CLASSES_HPP_