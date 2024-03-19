#ifndef ELEMENT_CLASSES_HPP_
#define ELEMENT_CLASSES_HPP_
#include <QtCore>
#include <utility>

class tag {
    quint32 m_tag_id;
    QString m_name;

public:
    explicit tag(quint32 tag_id, QString name);
};

class card {
    quint32 m_card_id;
    quint32 m_list_id;
    QString m_name;
    QString m_description;
    QVector<tag> m_tags;

public:
    explicit card(quint32 card_id, quint32 list_id, QString name, QString description);
};

class list {
    quint32 m_list_id;
    quint32 m_board_id;
    QString m_name;
    QString m_description;
    QVector<card> m_cards;

public:
    explicit list(quint32 list_id, quint32 board_id, QString name, QString description);

    void print_data() const {
        qDebug() << m_list_id << m_board_id << m_name << m_description;
    }
};

class board {
    quint32 m_board_id;
    quint32 m_user_id;
    QString m_name;
    QString m_description;
    QVector<list> m_lists;

public:
    explicit board(quint32 board_id, quint32 user_id, QString name, QString description);

    void print_data() const {
        qDebug() << m_board_id << ' ' << m_name;
    }
};

class user {
    quint32 m_user_id;
    QString m_name;

public:
    explicit user(quint32 user_id, QString name);

    void print_data() const {
        qDebug() << m_user_id << m_name;
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

user::user(quint32 user_id, QString name) : m_user_id(user_id), m_name(std::move(name)) {
}



board::board(quint32 board_id, quint32 user_id, QString name, QString description)
    : m_board_id(board_id),
      m_user_id(user_id),
      m_name(std::move(name)),
      m_description(std::move(description)) {
}

list::list(quint32 list_id, quint32 board_id, QString name, QString description)
        : m_list_id(list_id),
          m_board_id(board_id),
          m_name(std::move(name)),
          m_description(std::move(description)) {
}

card::card(quint32 card_id, quint32 list_id, QString name, QString description)
        : m_card_id(card_id),
          m_list_id(list_id),
          m_name(std::move(name)),
          m_description(std::move(description)) {
}

tag::tag(quint32 tag_id, QString name) : m_tag_id(tag_id), m_name(std::move(name)) {
}

#endif  // ELEMENT_CLASSES_HPP_
