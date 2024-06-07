#ifndef BOARDMENU_HPP_
#define BOARDMENU_HPP_

#include <QAbstractListModel>
#include <QScopedPointer>
#include <QVector>
#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include "boardmodel.hpp"
#include "element_classes.hpp"

class BoardMenu : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ get_count NOTIFY countChanged)
public:
    explicit BoardMenu(QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parentclear = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)
        const override;
    int get_count();
    void create_board(
        const QString &name,
        const QString &description,
        quint32 id,
        quint32 user_id,
        const QString &link = "",
        bool is_remote = false
    );
    void create_board(const board &board);
    std::pair<quint32, bool> get_info(int board_index) const;
    std::pair<quint32, bool> delete_board(int board_index);
    BoardModel *load(const board &loaded_board);
    bool is_board_loaded(quint32 id) const;
    void unload_remote_boards();
    void update_board(int index, const QString &field, const QString &value);
    void create_list(quint32 board_id, const list &list);
    void create_card(quint32 board_id, quint32 list_id, const card &card);
    void create_tag(
        quint32 board_id,
        quint32 list_id,
        quint32 card_id,
        const tag &tag
    );

    void delete_command(
        quint32 board_id,
        quint32 list_id,
        quint32 card_id,
        quint32 tag_id
    );
    void update_command(
        quint32 board_id,
        quint32 list_id,
        quint32 card_id,
        const QString &field,
        const QString &new_value
    );
    void move_command(
        quint32 board_id,
        quint32 old_list_id,
        quint32 new_list_id,
        quint32 card_id,
        int new_index
    );
    nlohmann::json board_to_json(int index) const;
    bool board_already_connected(const QString &link) const;

    friend class Client;
signals:
    void countChanged();

private:
    QVector<board> m_boards;
    std::unordered_map<quint32, std::unique_ptr<BoardModel>> m_loaded_boards;
    bool is_local_loaded = false;

    enum BoardRoles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        LinkRole,
        TypeRole
    };
};

#endif  // BOARDMENU_HPP_
