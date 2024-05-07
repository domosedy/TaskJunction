#include "boardmenu.hpp"
#include "element_classes.hpp"

BoardMenu::BoardMenu(QObject *parent) : QAbstractListModel(parent) {
}

int BoardMenu::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_boards.count();
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
    const board &board = m_boards.at(index.row());

    switch (role) {
        case BoardRoles::NameRole:
            return {board.m_name};
        case BoardRoles::DescriptionRole:
            return {board.m_description};
        default:
            return {};
    }
}

void BoardMenu::add_board(
    QString &name,
    QString &description,
    quint32 id,
    quint32 user_id
) {
    beginInsertRows(QModelIndex(), m_boards.size(), m_boards.size());
    m_boards.append(board(id, user_id, name, description));
    endInsertRows();

    m_board_id_to_index[id] = m_boards.size();

    emit countChanged();
}

void BoardMenu::add_board(const board &board) {
    beginInsertRows(QModelIndex(), m_boards.size(), m_boards.size());
    m_boards.append(board);
    endInsertRows();

    m_board_id_to_index[board.m_board_id] = m_boards.size();

    emit countChanged();
}

std::pair<quint32, bool> BoardMenu::get_info(int board_index) const {
    return {
        m_boards[board_index].m_board_id, m_boards[board_index].m_is_remote
    };
}

int BoardMenu::get_count() {
    return m_boards.count();
}

BoardModel *BoardMenu::load(int index, const board &loaded_board) {
    quint32 id = loaded_board.m_board_id;

    m_board_id_to_index[id] = index;
    m_loaded_boards[id] = std::make_unique<BoardModel>(loaded_board);
    return m_loaded_boards[id].get();
}

std::pair<quint32, bool> BoardMenu::delete_board(int board_index) {
    auto info = get_info(board_index);
    beginRemoveRows(QModelIndex(), board_index, board_index);
    m_boards.remove(board_index);
    endRemoveRows();

    m_board_id_to_index.remove(info.first);
    m_loaded_boards.erase(info.first);

    emit countChanged();

    return info;
}

void BoardMenu::unload_remote_boards() {
    int index = 0;
    for (auto it = m_boards.begin(); it != m_boards.end();) {
        if (it->m_is_remote) {
            quint32 id = it->m_board_id;
            m_board_id_to_index.remove(id);
            m_loaded_boards.erase(id);
            beginRemoveRows(QModelIndex(), index, index);
            it = m_boards.erase(it);
            endRemoveRows();
        } else {
            it++;
            index++;
        }
    }

    emit countChanged();
}
