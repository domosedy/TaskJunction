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
    const nlohmann::json &cards = list_json["lists"];
    int index = 0;
    for (const auto &card : cards) {
        QString card_name = QString::fromStdString(card["name"]);
        QString card_description = QString::fromStdString(card["description"]);
        quint32 card_id = card["id"];
        m_cards.emplace_back(card_id, 0, card_name, card_description);
        m_index_by_id[card_id] = index++;
    }
    emit countChanged();
}

ListModel::ListModel(QObject *parent, const list &list_base)
    : ListModel(parent) {
    m_list_id = list_base.m_list_id;
    m_board_id = list_base.m_board_id;
    m_name = list_base.m_name;
    m_description = list_base.m_description;
    m_cards = list_base.m_cards;
    int index = 0;
    for (auto &card : m_cards) {
        m_index_by_id[card.m_card_id] = index++;
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
        // roles[CardRoles::IdRole] = "card_id";
        // roles[CardRoles::ListIdRole] = "list_id";
        roles[CardRoles::CardIndex] = "cardIndex";
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
        // case CardRoles::IdRole:
        //     return {card.m_card_id};
        // case CardRoles::ListIdRole:
        //     return {card.m_list_id};
        case CardRoles::CardIndex:
            return {index.row()};
        default:
            return {};
    }
}

void ListModel::create_card(QString &name, QString &description) {
    if (name == "") {
        name = "New card";
    }
    beginInsertRows(QModelIndex(), m_cards.size(), m_cards.size());
    m_cards.append(card(0, 0, name, description));
    endInsertRows();

    emit countChanged();
}

void ListModel::create_card(const card &new_card, const int index) {
    m_index_by_id[new_card.m_card_id] = m_cards.size();
    const int pos = (index == -1 ? m_cards.size() : index);
    beginInsertRows(QModelIndex(), pos, pos);
    if (index == -1) {
        m_cards.append(new_card);
    } else {
        m_cards.insert(index, new_card);
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

int ListModel::get_count() {
    return m_cards.count();
}

quint32 ListModel::get_card_id(const int index) const {
    return m_cards[index].m_card_id;
}

void ListModel::update_card_name(int card_index, QString &name) {
    m_cards[card_index].m_name = name;
    emit dataChanged(
        this->index(card_index, 0), this->index(card_index, 0),
        {CardRoles::NameRole}
    );
}

void ListModel::update_card_description(int card_index, QString &description) {
    m_cards[card_index].m_description = description;
    emit dataChanged(
        this->index(card_index, 0), this->index(card_index, 0),
        {CardRoles::DescriptionRole}
    );
}

bool ListModel::setData(
    const QModelIndex &index,
    const QVariant &data,
    int role
) {
    if (role != Qt::DisplayRole) {
        return false;
    }
    m_cards.insert(index.row(), data.value<card>());
    return true;
}

bool ListModel::insertRows(int row, int count, const QModelIndex &parent) {
    if (parent.isValid()) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        m_cards.insert(row + i, card(0, 0, "", ""));
    }
    return true;
}

bool ListModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (parent.isValid()) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_cards.removeAt(row);
    }
    endRemoveRows();
    return true;
}

QStringList ListModel::mimeTypes() const {
    QStringList types;
    types << ListModel::MimeType;
    return types;
}

bool ListModel::
    canDropMimeData(const QMimeData *data, Qt::DropAction action, int, int, const QModelIndex &)
        const {
    if (action != Qt::MoveAction || !data->hasFormat(ListModel::MimeType)) {
        return false;
    }
    return true;
}

QMimeData *ListModel::mimeData(const QModelIndexList &indices) const {
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for (const QModelIndex &index : indices) {
        if (index.isValid()) {
            QString name = data(index, CardRoles::NameRole).toString();
            QString description =
                data(index, CardRoles::DescriptionRole).toString();
            quint32 card_id = data(index, CardRoles::IdRole).toUInt();
            quint32 list_id = data(index, CardRoles::ListIdRole).toUInt();
            stream << name << description << card_id << list_id;
        }
    }
    mimeData->setData(ListModel::MimeType, encodedData);
    return mimeData;
}

bool ListModel::dropMimeData(
    const QMimeData *data,
    Qt::DropAction action,
    int row,
    int column,
    const QModelIndex &parent
) {
    qDebug() << "ITS ALIVE";
    if (!canDropMimeData(data, action, row, column, parent)) {
        return false;
    }

    if (action == Qt::IgnoreAction) {
        return true;
    } else if (action != Qt::MoveAction) {
        return false;
    }

    QByteArray encodedData = data->data(ListModel::MimeType);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QVector<card> new_cards;
    int rows = 0;

    while (!stream.atEnd()) {
        QString name, description;
        quint32 card_id, list_id;
        stream >> name >> description >> card_id >> list_id;
        new_cards.emplace_back(card_id, list_id, name, description);
        ++rows;
    }

    insertRows(row, rows, QModelIndex());
    for (const card &new_card : new_cards) {
        QModelIndex idx = index(row, 0, QModelIndex());
        setData(idx, QVariant::fromValue<card>(new_card));
        row++;
    }

    return true;
}

Qt::DropActions ListModel::supportedDropActions() const {
    return Qt::MoveAction;
}

Qt::DropActions ListModel::supportedDragActions() const {
    return Qt::MoveAction;
}

Qt::ItemFlags ListModel::flags(const QModelIndex &index) const {
    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    } else {
        return Qt::ItemIsDropEnabled;
    }
}

card ListModel::remove(const int index) {
    card removed = std::move(m_cards[index]);
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
