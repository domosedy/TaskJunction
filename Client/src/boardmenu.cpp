#include "boardmenu.hpp"
#include "element_classes.hpp"

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
    const board &board = boards.at(index.row());

    switch (role) {
        case BoardRoles::NameRole:
            return {board.m_name};
        case BoardRoles::DescriptionRole:
            return {board.m_description};
        default:
            return {};
    }
}

void BoardMenu::create_board(QString &name, QString &description, quint32 id) {
    beginInsertRows(QModelIndex(), boards.size(), boards.size());
    boards.append(board(id,0,name, description));
    endInsertRows();

    emit countChanged();
}

void BoardMenu::add_board(const board &board) {
    beginInsertRows(QModelIndex(), boards.size(), boards.size());
    boards.append(board);
    endInsertRows();

    emit countChanged();
}

quint32 BoardMenu::get_id(int board_index) const {
    return boards[board_index].m_board_id;
}

int BoardMenu::get_count() {
    return boards.count();
}
