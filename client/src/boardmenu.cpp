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

void BoardMenu::create_board(
    QString &name,
    QString &description,
    quint32 id,
    quint32 user_id,
    bool is_remote
) {
    m_board_id_to_index[id] = m_boards.size();    
    beginInsertRows(QModelIndex(), m_boards.size(), m_boards.size());
    m_boards.append(board(id, user_id, name, description, is_remote));
    endInsertRows();

    emit countChanged();
}

void BoardMenu::create_board(const board &board) {
    m_board_id_to_index[board.m_board_id] = m_boards.size();
    beginInsertRows(QModelIndex(), m_boards.size(), m_boards.size());
    m_boards.append(board);
    endInsertRows();

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
    qDebug() << "Unloading: " << m_boards.size();
    for (auto it = m_boards.begin(); it != m_boards.end();) {
        qDebug() << it->m_board_id << ' ' << it->m_is_remote << ' ' << it->m_name;
        if (it->m_is_remote) {
            quint32 id = it->m_board_id;
            m_board_id_to_index.remove(id);
            m_loaded_boards.erase(id);
            beginRemoveRows(QModelIndex(), index, index);
            it = m_boards.erase(it);
            endRemoveRows();
        } else {
            m_board_id_to_index[it->m_board_id] = index;
            it++;
            index++;
        }
    }
    qDebug() << "END: " << m_boards.size();
    emit countChanged();
}

void BoardMenu::update_board(
    int index,
    const QString &field,
    const QString &value
) {
    quint32 id = m_boards[index].m_board_id;
    if (field == "name") {
        m_boards[index].m_name = value;
        if (m_loaded_boards.find(id) != m_loaded_boards.end()) {
            m_loaded_boards[id]->m_name = value;
        }
    } else {
        m_boards[index].m_description = value;
        if (m_loaded_boards.find(id) != m_loaded_boards.end()) {
            m_loaded_boards[id]->m_description = value;
        }
    }
}

bool BoardMenu::is_board_loaded(quint32 id) const {
    return (m_loaded_boards.find(id) != m_loaded_boards.end());
}

void BoardMenu::create_list(quint32 board_id, const list &list) {
    m_loaded_boards[board_id]->create_list(list);
}

void BoardMenu::create_card(
    quint32 board_id,
    quint32 list_id,
    const card &card
) {
    m_loaded_boards[board_id]->create_card(list_id, card);
}

void BoardMenu::create_tag(
    quint32 board_id,
    quint32 list_id,
    quint32 card_id,
    const tag &tag
) {
    m_loaded_boards[board_id]->create_tag(list_id, card_id, tag);
}

void BoardMenu::delete_command(
    quint32 board_id,
    quint32 list_id,
    quint32 card_id,
    quint32 tag_id
) {
    auto [list_idx, card_idx, tag_idx] =
        m_loaded_boards[board_id]->get_indices(list_id, card_id, tag_id);
    m_loaded_boards[board_id]->delete_command(list_idx, card_idx, tag_idx);
}

void BoardMenu::update_command(
    quint32 board_id,
    quint32 list_id,
    quint32 card_id,
    const QString &field,
    const QString &new_value
) {
    auto [list_idx, card_idx, _] =
        m_loaded_boards[board_id]->get_indices(list_id, card_id, 0);
    m_loaded_boards[board_id]->update_command(
        list_idx, card_idx, field, new_value
    );
}

void BoardMenu::move_command(
    quint32 board_id,
    quint32 old_list_id,
    quint32 new_list_id,
    quint32 card_id,
    int new_idx
) {
    int old_list_idx, from_idx, new_list_idx;
    std::tie(old_list_idx, from_idx, std::ignore) =
        m_loaded_boards[board_id]->get_indices(old_list_id, card_id, 0);
    std::tie(new_list_idx, std::ignore, std::ignore) =
        m_loaded_boards[board_id]->get_indices(new_list_id, 0, 0);
    m_loaded_boards[board_id]->move(
        from_idx, new_idx, old_list_idx, new_list_idx
    );
}
