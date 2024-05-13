#include "listmodel.hpp"
#include <QIODevice>
#include <QMimeData>
#include <QString>
#include "element_classes.hpp"

ListModel::ListModel(QObject *parent) : QAbstractListModel(parent) {
}

ListModel::ListModel(QObject *parent, const nlohmann::json &list_json)
    : ListModel(parent) {
    m_list_id = list_json["id"];
    m_name = QString::fromStdString(list_json["name"]);
    const nlohmann::json &cards = list_json["cards"];
    //int index = 0;
    for (const auto &card : cards) {
        m_cards.append(new CardModel(this, card));
        //m_index_by_id[card_id] = index++;
    }
    emit countChanged();
}

ListModel::ListModel(QObject *parent, const list &list_base)
    : ListModel(parent) {
    m_list_id = list_base.m_list_id;
    m_board_id = list_base.m_board_id;
    m_name = list_base.m_name;
    m_description = list_base.m_description;
    // m_cards = list_base.m_cards;
    // int index = 0;
    for (auto &card : list_base.m_cards) {
        m_cards.append(new CardModel(this, card));
    }
    emit countChanged();
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
        roles[CardRoles::CardIndex] = "cardIndex";
        roles[CardRoles::ModelRole] = "cardModel";
    }

    return roles;
}

QVariant ListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > rowCount(index)) {
        return {};
    }
    CardModel* card = m_cards.at(index.row());

    switch (role) {
        case CardRoles::NameRole:
            return {card->m_name};
        case CardRoles::DescriptionRole:
            return {card->m_description};
        case CardRoles::CardIndex:
            return {index.row()};
        case CardRoles::ModelRole:
            return QVariant::fromValue<QObject *>(card);
        default:
            return {};
    }
}

void ListModel::create_card(QString &name, QString &description) {
    if (name == "") {
        name = "New card";
    }
    beginInsertRows(QModelIndex(), m_cards.size(), m_cards.size());
    m_cards.append(new CardModel(this, name, description));
    endInsertRows();

    emit countChanged();
}

void ListModel::create_card(const card &new_card, const int index) {
    //m_index_by_id[new_card.m_card_id] = m_cards.size();
    const int pos = (index == -1 ? m_cards.size() : index);
    beginInsertRows(QModelIndex(), pos, pos);
    if (index == -1) {
        m_cards.append(new CardModel(this, new_card));
    } else {
        m_cards.insert(index, new CardModel(this, new_card));
    }
    endInsertRows();

    emit countChanged();
}

void ListModel::delete_card(int index) {
    beginResetModel();
    m_cards.move(index, m_cards.size() - 1);
    endResetModel();
    beginRemoveRows(QModelIndex(), m_cards.size() - 1, m_cards.size() - 1);
    m_cards.pop_back();
    endRemoveRows();

    emit countChanged();
}

void ListModel::delete_tag(const int card_index, const int tag_index) {
    m_cards[card_index]->delete_tag(tag_index);
}

int ListModel::get_count() {
    return m_cards.count();
}

quint32 ListModel::get_card_id(const int index) const {
    return m_cards[index]->m_card_id;
}

quint32 ListModel::get_tag_id(const int card_index, const int tag_index) const {
    return m_cards[card_index]->get_tag_id(tag_index);
}

void ListModel::update_card(
    int card_index,
    const QString &field,
    const QString &value
) {
    if (field == "name") {
        m_cards[card_index]->m_name = value;
        emit dataChanged(
            this->index(card_index, 0), this->index(card_index, 0),
            {CardRoles::NameRole}
        );
    } else {
        m_cards[card_index]->m_description = value;
        emit dataChanged(
            this->index(card_index, 0), this->index(card_index, 0),
            {CardRoles::DescriptionRole}
        );
    }
}

CardModel* ListModel::remove(const int index) {
    CardModel* removed = m_cards[index];
    m_cards.remove(index);
    emit countChanged();
    return removed;
}

void ListModel::move(int from, int to) {
    if (from >= 0 && from < rowCount() && to >= 0 && to < rowCount() &&
        from != to) {
        if (from == to - 1) {
            to = from++;
        }
        beginResetModel();
        m_cards.move(from, to);
        endResetModel();
    }
}

void ListModel::create_card(CardModel* card, int index) {
    const int pos = (index == -1 ? m_cards.size() : index);
    beginInsertRows(QModelIndex(), pos, pos);
    if (index == -1) {
        m_cards.append(card);
    } else {
        m_cards.insert(index, card);
    }
    endInsertRows();

    emit countChanged();    
}

void ListModel::create_tag(int index, const tag &new_tag) {
    m_cards[index]->create_tag(new_tag);
}
