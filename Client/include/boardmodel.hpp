#ifndef BOARDMODEL_HPP_
#define BOARDMODEL_HPP_

#include <QAbstractListModel>
#include <QVector>
#include "list.hpp"

class BoardModel: public QAbstractListModel {
    Q_OBJECT  
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    BoardModel(QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void add_list();
    int get_count();
signals:
    void countChanged();
private:
    QVector<List> lists;

    enum ListRoles {
        NameRole = Qt::UserRole+1,
        DescriptionRole,
        ModelRole
    };
};

#endif