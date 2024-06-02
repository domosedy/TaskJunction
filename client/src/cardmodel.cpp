#include "cardmodel.hpp"

CardModel::CardModel(QObject *parent) : QAbstractListModel(parent) {
}

CardModel::CardModel(QObject *parent, const nlohmann::json &card_json)
    : CardModel(parent) {
    m_card_id = card_json["id"];
    m_name = QString::fromStdString(card_json["name"]);
    m_description = QString::fromStdString(card_json["description"]);
    const nlohmann::json &tags = card_json["tags"];
    int index = 0;
    for (const auto &tag : tags) {
        QString tag_name = QString::fromStdString(tag["name"]);
        quint32 tag_id = tag["id"];
        m_tags.emplace_back(tag_id, tag_name);
        m_index_by_id[tag_id] = index++;
    }
    emit countChanged();
}

CardModel::CardModel(QObject *parent, const card &card_base)
    : CardModel(parent) {
    m_list_id = card_base.m_list_id;
    m_card_id = card_base.m_card_id;
    m_name = card_base.m_name;
    m_description = card_base.m_description;
    m_tags = card_base.m_tags;
    qDebug() << m_tags.size();
    int index = 0;
    for (auto &tag : m_tags) {
        m_index_by_id[tag.m_tag_id] = index++;
    }
    emit countChanged();
}

CardModel::CardModel(
    QObject *parent,
    QString name,
    QString description,
    quint32 id,
    quint32 list_id
)
    : QAbstractListModel(parent), card(id, list_id, name, description) {
}

int CardModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_tags.count();
}

QHash<int, QByteArray> CardModel::roleNames() const {
    static QHash<int, QByteArray> roles;
    if (roles.empty()) {
        roles[TagRoles::NameRole] = "name";
    }

    return roles;
}

QVariant CardModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > rowCount(index)) {
        return {};
    }
    const tag &tag = m_tags.at(index.row());

    switch (role) {
        case TagRoles::NameRole:
            return {tag.m_name};
        default:
            return {};
    }
}

void CardModel::create_tag(const tag &new_tag) {
    m_index_by_id[new_tag.m_tag_id] = m_tags.size();
    beginInsertRows(QModelIndex(), m_tags.size(), m_tags.size());
    m_tags.append(new_tag);
    endInsertRows();

    emit countChanged();
}

void CardModel::delete_tag(const int index) {
    beginResetModel();
    m_tags.move(index, m_tags.size() - 1);
    endResetModel();
    beginRemoveRows(QModelIndex(), m_tags.size() - 1, m_tags.size() - 1);
    m_tags.pop_back();
    endRemoveRows();

    emit countChanged();
}

int CardModel::get_count() {
    return m_tags.count();
}

quint32 CardModel::get_tag_id(const int index) const {
    return m_tags[index].m_tag_id;
}

void CardModel::update_card(int tag_index, const QString &value) {
    m_tags[tag_index].m_name = value;
    emit dataChanged(
        this->index(tag_index, 0), this->index(tag_index, 0),
        {TagRoles::NameRole}
    );
}

int CardModel::get_tag_idx(quint32 tag_id) const {
    return m_index_by_id[tag_id];
}

nlohmann::json CardModel::to_json() const {
    nlohmann::json data = {
        {"type", "card"},
        {"id", m_card_id},
        {"name", m_name.toStdString().c_str()},
        {"description", m_description.toStdString().c_str()},
        {"tags", nlohmann::json::array()}
    };
    for (auto &tag : m_tags) {
        data["tags"].push_back(nlohmann::json::parse(tag.to_json()));
    }
    return data;
}
