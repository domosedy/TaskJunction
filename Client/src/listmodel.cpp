#include "listmodel.hpp"
#include <QString>
#include "element_classes.hpp"

ListModel::ListModel(QObject *parent) : QAbstractListModel(parent) {
}

ListModel::ListModel(QObject *parent, const nlohmann::json &list_json)
    : QAbstractListModel(parent) {
    m_list_id = list_json["id"];
    m_name = QString::fromStdString(list_json["name"]);
    m_description = QString::fromStdString(list_json["description"]);
    const nlohmann::json &cards = list_json["lists"];
    for (const auto &card : cards) {
        QString card_name = QString::fromStdString(card["name"]);
        QString card_description = QString::fromStdString(card["description"]);
        quint32 card_id = card["id"];
        m_cards.emplace_back(card_id, 0, card_name, card_description);
    }
}

ListModel::ListModel(QObject *parent, const list &list_base)
    : QAbstractListModel(parent) {
    m_list_id = list_base.m_list_id;
    m_board_id = list_base.m_board_id;
    m_name = list_base.m_name;
    m_description = list_base.m_description;
    m_cards = list_base.m_cards;
}

ListModel::ListModel(
    QObject *parent,
    QString name,
    QString description,
    quint32 id,
    quint32 board_id
)
    : QAbstractListModel(parent), list(id, board_id, name, description) {
}

int ListModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_cards.count();
}

QHash<int, QByteArray> ListModel::roleNames() const {
    static QHash<int, QByteArray> roles;
    if (roles.empty()) {
        roles[CardRoles::NameRole] = "name";
        roles[CardRoles::DescriptionRole] = "description";
    }

    return roles;
}

QVariant ListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > rowCount(index)) {
        return {};
    }
    const card &card = m_cards.at(index.row());

    switch (role) {
        case CardRoles::NameRole:
            return {card.m_name};
        case CardRoles::DescriptionRole:
            return {card.m_description};
        default:
            return {};
    }
}

void ListModel::create_card(QString &name, QString &description) {
    if (name == "") {
        name = "New card";
    }

    beginInsertRows(QModelIndex(), m_cards.size(), m_cards.size());
    m_cards.append(card(0,0,name, description));
    endInsertRows();

    emit countChanged();
}

void ListModel::create_card(const card &new_card) {
    beginInsertRows(QModelIndex(), m_cards.size(), m_cards.size());
    m_cards.append(new_card);
    endInsertRows();

    emit countChanged();
}

void ListModel::delete_card(int index) {
    beginRemoveRows(QModelIndex(), index, index);
    m_cards.remove(index);
    endRemoveRows();

    emit countChanged();
}

int ListModel::get_count() {
    return m_cards.count();
}

quint32 ListModel::get_card_id(const int index) const {
    return m_cards[index].m_card_id;
}
