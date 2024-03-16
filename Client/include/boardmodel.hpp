#ifndef BOARDMODEL_HPP_
#define BOARDMODEL_HPP_

#include <QAbstractListModel>
#include <QMap>
#include <QVector>
#include <nlohmann/json.hpp>
#include "base_classes.hpp"
#include "listmodel.hpp"

class BoardModel : public QAbstractListModel, public Board {
    Q_OBJECT
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    explicit BoardModel(QObject *parent = nullptr);
    BoardModel(QObject *parent, const nlohmann::json &board);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)
        const override;
    Q_INVOKABLE void create_list(QString &name);
    Q_INVOKABLE void delete_list(int index);
    void create_card(int list_index, QString &name, QString &description);
    int get_count() const;

signals:
    void countChanged();
private slots:
    void create_card(quint16 list_id, Card &new_card);

private:
    QVector<ListModel *> m_lists;
    QMap<quint16, int> m_index_by_id;

    enum ListRoles { NameRole = Qt::UserRole + 1, DescriptionRole, ModelRole };
};

#endif
