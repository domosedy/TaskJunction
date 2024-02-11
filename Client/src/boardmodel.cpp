#include "boardmodel.hpp"

BoardModel::BoardModel(QObject *parent): QAbstractListModel(parent) {

}

int BoardModel::rowCount(const QModelIndex &) const
{
    return static_cast<int>(lists.size());
}

QVariant BoardModel::data([[maybe_unused]] const QModelIndex &index, [[maybe_unused]] int role) const {
    return {};
}