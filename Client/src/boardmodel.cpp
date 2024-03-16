#include "boardmodel.hpp"
#include <QVariant>
#include <nlohmann/json.hpp>
#include "base_classes.hpp"
#include "listmodel.hpp"

namespace {
using json = nlohmann::json;
}

BoardModel::BoardModel(QObject *parent) : QAbstractListModel(parent) {
    connect(
        parent, SIGNAL(create_card(quint16, Card &)), this,
        SLOT(create_card(quint16, Card &))
    );
}

BoardModel::BoardModel(QObject *parent, const json &board)
    : BoardModel(parent) {
    m_id = board["id"];
    m_name = QString::fromStdString(board["name"]);
    m_description = QString::fromStdString(board["description"]);
    const json &lists = board["lists"];
    for (const auto &list : lists) {
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
            return {list->get_name()};
        case ListRoles::DescriptionRole:
            return {list->get_description()};
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

void BoardModel::delete_list(int index) {
    beginRemoveRows(QModelIndex(), index, index);
    delete m_lists[index];
    m_lists.remove(index);
    endRemoveRows();

    emit countChanged();
}

void BoardModel::create_card(
    int list_index,
    QString &name,
    QString &description
) {
    m_lists[list_index]->create_card(name, description);
}

void BoardModel::create_card(quint16 list_id, Card &new_card) {
    int index = m_index_by_id.value(list_id, -1);
    if (index == -1) {
        qDebug() << "Wrong list";
        return;
    }
    m_lists[index]->create_card(new_card);
}

int BoardModel::get_count() const {
    return m_lists.count();
}
