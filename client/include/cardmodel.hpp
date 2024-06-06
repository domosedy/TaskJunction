#ifndef CARDMODEL_HPP_
#define CARDMODEL_HPP_

#include <QAbstractListModel>
#include <QMap>
#include <QVector>
#include <nlohmann/json.hpp>
#include "element_classes.hpp"

class CardModel : public QAbstractListModel, public card {
    Q_OBJECT
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    explicit CardModel(QObject *parent = nullptr);
    CardModel(QObject *parent, const nlohmann::json &card_json);
    CardModel(QObject *parent, const card &card_base);
    CardModel(
        QObject *parent,
        QString name,
        QString description,
        quint32 id = 0,
        quint32 list_id = 0
    );
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)
        const override;

    void create_tag(const tag &new_tag);
    void delete_tag(const int index);
    void update_card(int tag_index, const QString &value);
    int get_count();
    quint32 get_tag_id(const int index) const;
    int get_tag_idx(quint32 tag_id) const;
    nlohmann::json to_json() const;

signals:
    void countChanged();

private:
    QMap<quint32, int> m_index_by_id;

    enum TagRoles {
        NameRole = Qt::UserRole + 1,
    };
};

#endif  // CARDMODEL_HPP_
