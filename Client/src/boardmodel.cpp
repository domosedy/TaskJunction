#include "boardmodel.hpp"
#include "listmodel.hpp"
#include "list.hpp"
#include <QVariant>

BoardModel::BoardModel(QObject *parent): QAbstractListModel(parent) {
    //qDebug() << "Created board!";
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
    //qDebug() << "Add list!";
    static int cnt = 1;
    QString name = QString::fromStdString("list_" + std::to_string(cnt));
    cnt++;    
    beginInsertRows(QModelIndex(), lists.size(), lists.size());
    lists.append(List(name, "", this));
    endInsertRows();

    emit countChanged();
}

void BoardModel::delete_list(int index) {
    //qDebug() << lists[index].get_name();
    beginRemoveRows(QModelIndex(), index, index);
    lists.remove(index);
    endRemoveRows();

    emit countChanged();
}

bool BoardModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || index.row() > rowCount(index))
        return false;
    if (role == Qt::EditRole) {
        lists[index.row()].get_name() = value.toString();
    }
    return false;
}
Qt::ItemFlags BoardModel::flags(const QModelIndex &index) const {
    if (!index.isValid() || index.row() > rowCount(index))
        return Qt::NoItemFlags;
    return Qt::ItemIsEditable;
}

int BoardModel::get_count() {
    return lists.count();
}