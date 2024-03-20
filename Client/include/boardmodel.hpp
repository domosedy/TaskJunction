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
    BoardModel(QObject *parent, const nlohmann::json &board_json);
    BoardModel(QObject *parent, const board &board_base);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)
        const override;
    Q_INVOKABLE void create_list(QString &name);
    void create_list(const list &list_base);
    void create_card(int index, const card &new_card);
    void delete_list(int index);
    void delete_card(const int list_index, const int card_index);
    void create_card(int list_index, QString &name, QString &description);
    int get_count() const;
    quint32 get_list_id(const int index) const;
    quint32 get_card_id(const int list_index, const int card_index) const;

signals:
    void countChanged();

private:
    QVector<ListModel *> m_lists;
    QMap<quint32, int> m_index_by_id;

    enum ListRoles { NameRole = Qt::UserRole + 1, DescriptionRole, ModelRole };
};

#endif
