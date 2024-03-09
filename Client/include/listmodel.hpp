#ifndef LISTMODEL_HPP_
#define LISTMODEL_HPP_

#include "card.hpp"
#include <QAbstractListModel>
#include <QVector>

class ListModel: public QAbstractListModel {
    Q_OBJECT  
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    ListModel(QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void add_card(QString& name, QString& description);
    Q_INVOKABLE void delete_card(int index);
    int get_count();
signals:
    void countChanged();
private:
    QVector<Card> cards;

    enum CardRoles {
        NameRole = Qt::UserRole+1,
        DescriptionRole 
    };
};

#endif // LISTMODEL_HPP_