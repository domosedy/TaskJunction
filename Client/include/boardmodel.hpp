#ifndef BOARDMODEL_HPP_
#define BOARDMODEL_HPP_

#include <QAbstractListModel>
#include <QVector>
#include "list.hpp"

class BoardModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    BoardModel(QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)
        const override;
    bool setData(
        const QModelIndex &index,
        const QVariant &value,
        int role = Qt::EditRole
    ) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Q_INVOKABLE void add_list(QString &name);
    Q_INVOKABLE void delete_list(int index);
    void add_card(int list_index, QString& name, QString& description);
    int get_count() const;
    quint16 get_id() const;

signals:
    void countChanged();

private:
    QVector<List> lists;
    quint16 m_id = -1;

    enum ListRoles { NameRole = Qt::UserRole + 1, DescriptionRole, ModelRole };
};

#endif