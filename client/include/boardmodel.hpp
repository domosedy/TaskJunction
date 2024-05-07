#ifndef BOARDMODEL_HPP_
#define BOARDMODEL_HPP_

#include <QAbstractListModel>
#include <QMap>
#include <QVector>
#include <nlohmann/json.hpp>
#include "element_classes.hpp"
#include "listmodel.hpp"

class BoardModel : public QAbstractListModel, public board {
    Q_OBJECT
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    explicit BoardModel(QObject *parent = nullptr);
    BoardModel(const nlohmann::json &board_json, QObject *parent = nullptr);
    BoardModel(const board &board_base, QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)
        const override;
    void create_list(const list &list_base);
    void create_card(int index, const card &new_card);
    void create_card(quint32 list_id, const card &new_card);
    void delete_list(int index);
    void delete_card(const int list_index, const int card_index);
    void create_card(int list_index, QString &name, QString &description);
    void update_card_name(int list_index, int card_index, QString &name);
    void update_card_description(
        int list_index,
        int card_index,
        QString &description
    );
    void update_list_name(int list_index, QString &name);
    int get_count() const;
    quint32 get_list_id(const int index) const;
    quint32 get_card_id(const int list_index, const int card_index) const;
    void move(int from_card, int to_card, int from_list, int to_list);

signals:
    void countChanged();
    void moveRequest(int, int, int, int);

private:
    QVector<ListModel *> m_lists;
    QMap<quint32, int> m_index_by_id;

    enum ListRoles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        ModelRole,
        IndexRole
    };
};

#endif
