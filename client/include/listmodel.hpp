#ifndef LISTMODEL_HPP_
#define LISTMODEL_HPP_

#include <QAbstractListModel>
#include <QMap>
#include <QVector>
#include <nlohmann/json.hpp>
#include "cardmodel.hpp"
#include "element_classes.hpp"

class ListModel : public QAbstractListModel, public list {
    Q_OBJECT
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    explicit ListModel(QObject *parent = nullptr);
    ListModel(const nlohmann::json &list_json, QObject *parent);
    ListModel(const list &list_base, QObject *parent);
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

    Q_INVOKABLE void create_card(CardModel *card, int index);
    Q_INVOKABLE void create_card(QString &name, QString &description);
    Q_INVOKABLE void move(int from, int to);
    void create_card(const card &new_card, const int index = -1);
    void
    update_card(int card_index, const QString &field, const QString &value);
    int get_count();
    quint32 get_card_id(const int index) const;
    quint32 get_tag_id(const int card_index, const int tag_index) const;
    CardModel *remove(const int index);
    void create_tag(quint32 id, const tag &new_tag);
    void create_tag(int index, const tag &new_tag);
    std::pair<int, int> get_indices(quint32 card_id, quint32 tag_id) const;

    void delete_command(const int card_index, const int tag_index);
signals:
    void countChanged();

private:
    QVector<CardModel *> m_cards;

    enum CardRoles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        CardIndex,
        ModelRole
    };
};

#endif  // LISTMODEL_HPP_
