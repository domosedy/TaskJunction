#include "listmodel.hpp"
#include "card.hpp"

ListModel::ListModel(QObject *parent): QAbstractListModel(parent) {
    qDebug() << "Created listmodel!";
}

int ListModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return cards.count();
}

QHash<int, QByteArray> ListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[CardRoles::NameRole] = "name";
    roles[CardRoles::DescriptionRole] = "description";

    return roles;
}

QVariant ListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > rowCount(index))
        return {};
    const Card& card = cards.at(index.row());

    switch(role) {
        case CardRoles::NameRole:
            return {card.get_name()};
        case CardRoles::DescriptionRole:
            return {card.get_description()};
        default:
            return {};
    }
}

void ListModel::add_card() {
    qDebug() << "Add card!";
    beginInsertRows(QModelIndex(), cards.size(), cards.size());
    cards.append(Card("test"));
    endInsertRows();

    emit countChanged();
}

int ListModel::get_count() {
    return cards.count();
}