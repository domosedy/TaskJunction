#include "boardmodel.hpp"
#include <QVariant>
#include <nlohmann/json.hpp>
#include "element_classes.hpp"
#include "listmodel.hpp"

BoardModel::BoardModel(QObject *parent) : QAbstractListModel(parent) {
    connect(
        parent, SIGNAL(create_card(quint32, card &)), this,
        SLOT(create_card(quint32, card &))
    );
}

BoardModel::BoardModel(QObject *parent, const nlohmann::json &board_json)
    : BoardModel(parent) {
    m_board_id = board_json["id"];
    m_name = QString::fromStdString(board_json["name"]);
    m_description = QString::fromStdString(board_json["description"]);
    const nlohmann::json &lists = board_json["lists"];
    for (const auto &list : lists) {
        m_lists.push_back(new ListModel(this, list));
    }
}

BoardModel::BoardModel(QObject *parent, const board &board_base)
    : BoardModel(parent) {
    m_board_id = board_base.m_board_id;
    m_name = board_base.m_name;
    m_description = board_base.m_description;
    for (const auto &list : board_base.m_lists) {
        m_lists.push_back(new ListModel(this, list));
    }
}

int BoardModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_lists.count();
}

QHash<int, QByteArray> BoardModel::roleNames() const {
    static QHash<int, QByteArray> roles;
    if (roles.empty()) {
        roles[ListRoles::NameRole] = "name";
        roles[ListRoles::DescriptionRole] = "description";
        roles[ListRoles::ModelRole] = "listmodel";
    }

    return roles;
}

QVariant BoardModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > rowCount(index)) {
        return {};
    }
    ListModel *list = m_lists.at(index.row());

    switch (role) {
        case ListRoles::NameRole:
            return {list->m_name};
        case ListRoles::DescriptionRole:
            return {list->m_description};
        case ListRoles::ModelRole:
            return QVariant::fromValue<QObject *>(list);
        default:
            return {};
    }
}

void BoardModel::create_list(QString &name) {
    if (name == "") {
        name = "New list";
    }
    beginInsertRows(QModelIndex(), m_lists.size(), m_lists.size());
    m_lists.append(new ListModel(this, name, ""));
    endInsertRows();

    emit countChanged();
}

void BoardModel::create_list(const list &list_base) {
    beginInsertRows(QModelIndex(), m_lists.size(), m_lists.size());
    m_lists.append(new ListModel(this, list_base));
    endInsertRows();

    emit countChanged();
}

void BoardModel::delete_list(int index) {
    beginRemoveRows(QModelIndex(), index, index);
    delete m_lists[index];
    m_lists.remove(index);
    endRemoveRows();

    emit countChanged();
}

void BoardModel::delete_card(const int list_index, const int card_index) {
    m_lists[list_index]->delete_card(card_index);
}

void BoardModel::create_card(
    int list_index,
    QString &name,
    QString &description
) {
    m_lists[list_index]->create_card(name, description);
}

void BoardModel::create_card(int index, const card &new_card) {
    m_lists[index]->create_card(new_card);
}

int BoardModel::get_count() const {
    return m_lists.count();
}

quint32 BoardModel::get_list_id(const int index) const {
    return m_lists[index]->m_list_id;
}

quint32 BoardModel::get_card_id(const int list_index, const int card_index)
    const {
    return m_lists[list_index]->get_card_id(card_index);
}
