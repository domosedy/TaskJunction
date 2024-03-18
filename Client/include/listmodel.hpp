#ifndef LISTMODEL_HPP_
#define LISTMODEL_HPP_

#include <QAbstractListModel>
#include <QMap>
#include <QVector>
#include <nlohmann/json.hpp>
#include "element_classes.hpp"

class ListModel : public QAbstractListModel, public list {
    Q_OBJECT
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    explicit ListModel(QObject *parent = nullptr);
    ListModel(QObject *parent, const nlohmann::json &list_json);
    ListModel(
        QObject *parent,
        QString name,
        QString description,
        quint32 id = 0,
        quint32 board_id = 0
    );
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)
        const override;
    Q_INVOKABLE void create_card(QString &name, QString &description);
    Q_INVOKABLE void delete_card(int index);
    void create_card(card &new_card);
    int get_count();
signals:
    void countChanged();

private:
    QVector<card> m_cards;
    QMap<quint32, int> m_index_by_id;

    enum CardRoles { NameRole = Qt::UserRole + 1, DescriptionRole };
};

#endif  // LISTMODEL_HPP_
