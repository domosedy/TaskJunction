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
    quint32 m_tag_id;
    QString m_name;

    tag() = default;
    explicit tag(quint32 tag_id, QString name);
    std::string to_json() const;
};

struct card {
    quint32 m_card_id;
    quint32 m_list_id;
    QString m_name;
    QString m_description;
    QVector<tag> m_tags;

    card() = default;
    explicit card(quint32 card_id, quint32 list_id, QString name, QString description);
    card(QString name, QString description, quint32 id = 0, quint32 list_id = 0);
    std::string to_json() const;
};

struct list {
    quint32 m_list_id;
    quint32 m_board_id;
    QString m_name;
    QString m_description;
    QVector<card> m_cards;

    list() = default;
    explicit list(quint32 list_id, quint32 board_id, QString name, QString description);

    list(QString name, QString description, quint32 id = 0, quint32 board_id = 0);
    std::string to_json() const;
};

struct board {
    quint32 m_board_id;
    quint32 m_user_id;
    QString m_name;
    QString m_description;
    QVector<list> m_lists;

    board() = default;
    explicit board(quint32 board_id, quint32 user_id, QString name, QString description);
    board(QString name, QString description, quint32 id = 0);
    std::string to_json() const;
};

struct login {
    bool m_response;
    QVector<board> m_boards;

    std::string to_json() const;
};

struct user {
    quint32 m_user_id;
    QString m_name;

    explicit user(quint32 user_id, QString name);

    void print_data() const {
        qDebug() << m_user_id << m_name;
    }
};

struct error {
    std::string error_message;

    std::string to_json() const;
};

#endif  // ELEMENT_CLASSES_HPP_