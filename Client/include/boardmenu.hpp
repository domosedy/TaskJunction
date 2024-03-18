#ifndef BOARDMENU_HPP_
#define BOARDMENU_HPP_

#include <QAbstractListModel>
#include <QVector>
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
    void create_board(QString &name, QString &description, quint32 id);
    quint32 get_id(int board_index) const;
signals:
    void countChanged();
    void request_board(int index);

private:
    QVector<board> boards;

    enum BoardRoles { NameRole = Qt::UserRole + 1, DescriptionRole };
};

#endif  // BOARDMENU_HPP_
