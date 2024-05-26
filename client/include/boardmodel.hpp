#ifndef BOARDMODEL_HPP_
#define BOARDMODEL_HPP_

#include <QAbstractListModel>
#include <QMap>
#include <QVector>
#include <nlohmann/json.hpp>
#include <tuple>
#include "element_classes.hpp"
#include "listmodel.hpp"

class BoardModel : public QAbstractListModel, public board {
    Q_OBJECT
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    explicit BoardModel(QObject *parent = nullptr);
    BoardModel(
        const nlohmann::json &board_json,
        QObject *parent = nullptr
    );  // TODO WHERE IT NEEDED?
    BoardModel(const board &board_base, QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)
        const override;
    void create_list(const list &list_base);
    void create_card(int index, const card &new_card);
    void create_card(quint32 list_id, const card &new_card);
    void create_card(int list_index, QString &name, QString &description);
    void create_tag(quint32 list_id, quint32 card_id, const tag &new_tag);
    void create_tag(int list_index, int card_index, const tag &new_tag);
    int get_count() const;
    std::tuple<int, int, int>
    get_indices(quint32 list_id, quint32 card_id, quint32 tag_id) const;
    quint32 get_list_id(const int index) const;
    quint32 get_card_id(const int list_index, const int card_index) const;
    quint32 get_tag_id(
        const int list_index,
        const int card_index,
        const int tag_index
    ) const;
    void move(int from_card, int to_card, int from_list, int to_list);

    void delete_command(
        const int list_index,
        const int card_index,
        const int tag_index
    );
    void update_command(
        const int list_index,
        const int card_index,
        const QString &field,
        const QString &new_value
    );

signals:
    void countChanged();
    // void moveRequest(int, int, int, int);

private:
    QVector<quint32> m_ids;
    std::unordered_map<quint32, std::unique_ptr<ListModel>> m_lists;

    enum ListRoles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        ModelRole,
        IndexRole
    };
};

#endif
