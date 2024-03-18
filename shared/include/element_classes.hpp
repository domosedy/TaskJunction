#ifndef ELEMENT_CLASSES_HPP_
#define ELEMENT_CLASSES_HPP_

#include <QString>
#include <QVector>
#include <QVariant>

#include <string>
#include <concepts>

template<typename T>
concept has_to_json_method =
requires (T t) { 
    t.to_json(); 
};

struct tag {
    unsigned m_tag_id;
    QString m_name;

    tag() = default;
    explicit tag(QVector<QVariant> data);
    std::string to_json() const;
};

struct card {
    unsigned m_card_id;
    unsigned m_list_id;
    QString m_name;
    QString m_description;
    QVector<tag> m_tags;

    card() = default;
    explicit card(QVector<QVariant> data);
    std::string to_json() const;
};

struct list {
    unsigned m_list_id;
    unsigned m_board_id;
    QString m_name;
    QString m_description;
    QVector<card> m_cards;

    list() = default;
    explicit list(QVector<QVariant> data);
    std::string to_json() const;
};

struct board {
    unsigned m_board_id;
    QString m_name;
    QVector<list> m_lists;

    board() = default;
    explicit board(QVector<QVariant> data);
    std::string to_json() const;
};

struct login {
    bool m_response;
    QVector<std::pair<unsigned, QString>> m_boards;

    std::string to_json() const;
};

#endif  // ELEMENT_CLASSES_HPP_