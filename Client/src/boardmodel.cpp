#include "boardmodel.hpp"
#include "listmodel.hpp"
#include "list.hpp"
#include <QVariant>

BoardModel::BoardModel(QObject *parent): QAbstractListModel(parent) {
    qDebug() << "Created board!";
}

int BoardModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return lists.count();
}

QHash<int, QByteArray> BoardModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ListRoles::NameRole] = "name";
    roles[ListRoles::DescriptionRole] = "description";
    roles[ListRoles::ModelRole] = "listmodel";

    return roles;
}

QVariant BoardModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > rowCount(index))
        return {};
    const List& list = lists.at(index.row());

    switch(role) {
        case ListRoles::NameRole:
            return {list.get_name()};
        case ListRoles::DescriptionRole:
            return {list.get_description()};
        case ListRoles::ModelRole:
            return QVariant::fromValue<QObject *>(list.get_model());      
        default:
            return {};
    }
}

void BoardModel::add_list() {
    qDebug() << "Add list!";
    beginInsertRows(QModelIndex(), lists.size(), lists.size());
    lists.append(List("list" , "", this));
    endInsertRows();

    emit countChanged();
}

int BoardModel::get_count() {
    return lists.count();
}