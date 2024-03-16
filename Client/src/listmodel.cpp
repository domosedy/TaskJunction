#include "listmodel.hpp"
#include <QString>
#include "base_classes.hpp"

ListModel::ListModel(QObject *parent) : QAbstractListModel(parent) {
}

ListModel::ListModel(QObject *parent, const nlohmann::json &list)
    : QAbstractListModel(parent) {
    m_id = list["id"];
    m_name = QString::fromStdString(list["name"]);
    m_description = QString::fromStdString(list["description"]);
    const nlohmann::json &cards = list["lists"];
    for (const auto &card : cards) {
        QString card_name = QString::fromStdString(card["name"]);
        QString card_description = QString::fromStdString(card["description"]);
        quint16 card_id = card["id"];
        m_cards.emplace_back(card_name, card_description, card_id);
    }
}

ListModel::ListModel(
    QObject *parent,
    QString name,
    QString description,
    quint16 id
)
    : QAbstractListModel(parent), List(name, description, id) {
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
    const Card &card = m_cards.at(index.row());

    switch (role) {
        case CardRoles::NameRole:
            return {card.get_name()};
        case CardRoles::DescriptionRole:
            return {card.get_description()};
        default:
            return {};
    }
}

void ListModel::create_card(QString &name, QString &description) {
    if (name == "") {
        name = "New card";
    }

    beginInsertRows(QModelIndex(), m_cards.size(), m_cards.size());
    m_cards.append(Card(name, description));
    endInsertRows();

    emit countChanged();
}

void ListModel::create_card(Card &new_card) {
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