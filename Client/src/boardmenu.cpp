#include "boardmenu.hpp"
#include "base_classes.hpp"

BoardMenu::BoardMenu(QObject *parent) : QAbstractListModel(parent) {
}

int BoardMenu::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return boards.count();
}

QHash<int, QByteArray> BoardMenu::roleNames() const {
    static QHash<int, QByteArray> roles;
    if (roles.empty()) {
        roles[BoardRoles::NameRole] = "name";
        roles[BoardRoles::DescriptionRole] = "description";
    }

    return roles;
}

QVariant BoardMenu::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > rowCount(index)) {
        return {};
    }
    const Board &board = boards.at(index.row());

    switch (role) {
        case BoardRoles::NameRole:
            return {board.get_name()};
        case BoardRoles::DescriptionRole:
            return {board.get_description()};
        default:
            return {};
    }
}

void BoardMenu::create_board(QString &name, QString &description, quint16 id) {
    beginInsertRows(QModelIndex(), boards.size(), boards.size());
    boards.append(Board(name, description, id));
    endInsertRows();

    emit countChanged();
}

quint16 BoardMenu::get_id(int board_index) const {
    return boards[board_index].get_id();
}

int BoardMenu::get_count() {
    return boards.count();
}