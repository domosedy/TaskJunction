#include "boardmodel.hpp"
#include <QVariant>
#include <nlohmann/json.hpp>
#include "element_classes.hpp"
#include "listmodel.hpp"

BoardModel::BoardModel(QObject *parent) : QAbstractListModel(parent) {
}

BoardModel::BoardModel(const nlohmann::json &board_json, QObject *parent)
    : BoardModel(parent) {
    m_board_id = board_json["id"];
    m_name = QString::fromStdString(board_json["name"]);
    m_description = QString::fromStdString(board_json["description"]);
    m_link = QString::fromStdString(board_json["link"]);
    const nlohmann::json &lists = board_json["lists"];
    for (const auto &list : lists) {
        quint32 m_list_id = list["id"];
        m_ids.push_back(m_list_id);
        m_lists[m_list_id] = std::make_unique<ListModel>(list, this);
    }
}

BoardModel::BoardModel(const board &board_base, QObject *parent)
    : BoardModel(parent) {
    m_board_id = board_base.m_board_id;
    m_name = board_base.m_name;
    m_description = board_base.m_description;
    m_is_remote = board_base.m_is_remote;
    m_link = board_base.m_link;
    for (const auto &list : board_base.m_lists) {
        m_ids.push_back(list.m_list_id);
        m_lists[list.m_list_id] = std::make_unique<ListModel>(list, this);
    }
}

int BoardModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_lists.size();
}

QHash<int, QByteArray> BoardModel::roleNames() const {
    static QHash<int, QByteArray> roles;
    if (roles.empty()) {
        roles[ListRoles::NameRole] = "name";
        roles[ListRoles::DescriptionRole] = "description";
        roles[ListRoles::ModelRole] = "listmodel";
        roles[ListRoles::IndexRole] = "listIndex";
    }

    return roles;
}

QVariant BoardModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > rowCount(index)) {
        return {};
    }
    quint32 id = m_ids[index.row()];
    switch (role) {
        case ListRoles::NameRole:
            return {m_lists.at(id)->m_name};
        case ListRoles::DescriptionRole:
            return {m_lists.at(id)->m_description};
        case ListRoles::ModelRole:
            return QVariant::fromValue<QObject *>(m_lists.at(id).get());
        case ListRoles::IndexRole:
            return index.row();
        default:
            return {};
    }
}

void BoardModel::create_list(const list &list_base) {
    beginInsertRows(QModelIndex(), m_lists.size(), m_lists.size());
    m_ids.append(list_base.m_list_id);
    m_lists[list_base.m_list_id] = std::make_unique<ListModel>(list_base, this);
    endInsertRows();

    emit countChanged();
}

void BoardModel::delete_command(
    const int list_index,
    const int card_index,
    const int tag_index
) {
    if (tag_index != -1 || card_index != -1) {
        m_lists[m_ids[list_index]]->delete_command(card_index, tag_index);
    } else {
        beginRemoveRows(QModelIndex(), list_index, list_index);
        m_lists.erase(m_ids[list_index]);
        m_ids.remove(list_index);
        endRemoveRows();

        emit countChanged();
    }
}

void BoardModel::create_card(
    int list_index,
    QString &name,
    QString &description
) {
    m_lists[m_ids[list_index]]->create_card(name, description);
}

void BoardModel::create_card(int index, const card &new_card) {
    m_lists[m_ids[index]]->create_card(new_card);
}

void BoardModel::create_card(quint32 list_id, const card &new_card) {
    m_lists[list_id]->create_card(new_card);
}

void BoardModel::create_tag(quint32 list_id, quint32 card_id, const tag &tag) {
    m_lists[list_id]->create_tag(card_id, tag);
}

int BoardModel::get_count() const {
    return m_lists.size();
}

quint32 BoardModel::get_list_id(const int index) const {
    return m_ids[index];
}

std::tuple<int, int, int> BoardModel::get_indices(
    quint32 list_id,
    quint32 card_id,
    quint32 tag_id
) const {
    if (list_id == 0) {
        return {-1, -1, -1};
    }
    int list_idx = std::distance(
        m_ids.begin(),
        std::ranges::find_if(
            m_ids, [list_id](quint32 id) { return id == list_id; }
        )
    );
    auto [card_idx, tag_idx] =
        m_lists.at(list_id)->get_indices(card_id, tag_id);
    return {list_idx, card_idx, tag_idx};
}

quint32 BoardModel::get_card_id(const int list_index, const int card_index)
    const {
    return m_lists.at(m_ids[list_index])->get_card_id(card_index);
}

quint32 BoardModel::get_tag_id(
    const int list_index,
    const int card_index,
    const int tag_index
) const {
    return m_lists.at(m_ids[list_index])->get_tag_id(card_index, tag_index);
}

void BoardModel::update_command(
    const int list_index,
    const int card_index,
    const QString &field,
    const QString &new_value
) {
    if (card_index == -1) {
        m_lists[m_ids[list_index]]->m_name = new_value;
        emit dataChanged(
            this->index(list_index, 0), this->index(list_index, 0)
        );
    } else {
        m_lists[m_ids[list_index]]->update_card(card_index, field, new_value);
        emit dataChanged(
            this->index(0, list_index), this->index(0, list_index)
        );  // TODO Do I need this?
    }
}

void BoardModel::move(int from_card, int to_card, int from_list, int to_list) {
    // emit moveRequest(from_card, to_card, from_list, to_list);
    if (from_list == to_list) {
        m_lists[m_ids[from_list]]->move(from_card, to_card);
    } else {
        CardModel *moved = m_lists[m_ids[from_list]]->remove(from_card);
        moved->m_list_id = m_ids[to_list];
        m_lists[m_ids[to_list]]->create_card(moved, to_card);
    }
    emit dataChanged(this->index(from_list, 0), this->index(from_list, 0));
    emit dataChanged(this->index(to_list, 0), this->index(to_list, 0));
}

void BoardModel::create_tag(
    int list_index,
    int card_index,
    const tag &new_tag
) {
    m_lists[m_ids[list_index]]->create_tag(card_index, new_tag);
}

nlohmann::json BoardModel::to_json() const {
    nlohmann::json data = {
        {"type", "board"},
        {"id", m_board_id},
        {"name", m_name.toStdString().c_str()},
        {"description", m_description.toStdString().c_str()},
        {"lists", nlohmann::json::array()},
        {"link", m_link.toStdString().c_str()}
    };
    for (auto &list_id : m_ids) {
        data["lists"].push_back(m_lists.at(list_id)->to_json());
    }
    return data;
}
