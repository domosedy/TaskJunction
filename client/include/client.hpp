#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QMap>
#include <QObject>
#include <QWebSocket>
#include <nlohmann/json.hpp>
#include <string>
#include "boardmenu.hpp"
#include "boardmodel.hpp"
#include "database.hpp"

class Client : public QObject {
    Q_OBJECT
    Q_PROPERTY(BoardModel *current_board MEMBER m_current_board NOTIFY
                   boardChanged)
    Q_PROPERTY(BoardMenu *board_menu MEMBER m_board_menu NOTIFY menuChanged)
    Q_PROPERTY(ConnectionStatus connection_status MEMBER m_connection_status
                   NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString current_board_name READ get_current_board_name NOTIFY
                   boardChanged)

public:
    enum ConnectionStatus {
        Unauthorized,
        Authentification_failed,
        Unable_to_connect,
        Authorized
    };
    Q_ENUMS(ConnectionStatus)

    explicit Client(QObject *parent = nullptr);

    Q_INVOKABLE void login(
        QString username,
        QString password,
        QString server_ip,
        QString server_port
    );
    Q_INVOKABLE void connect_board(QString link);
    Q_INVOKABLE void load_local_boards();
    Q_INVOKABLE void load_board(int index);

    Q_INVOKABLE void
    create_card(int list_index, QString name, QString description);
    Q_INVOKABLE void create_list(QString name);
    Q_INVOKABLE void add_board(QString name, QString description, QString type);

    Q_INVOKABLE void delete_card(int list_index, int card_index);
    Q_INVOKABLE void delete_list(int list_index);
    Q_INVOKABLE void delete_board(int board_index);

    Q_INVOKABLE void update_card(
        int list_index,
        int card_index,
        QString field,
        QString value
    );  // qml throws segfault lol
    Q_INVOKABLE void update_list(int list_index, QString name);
    Q_INVOKABLE void
    update_board(int board_index, QString field, QString value);

    Q_INVOKABLE void
    move(int from_card, int to_card, int from_list, int to_list);

signals:
    void boardChanged();
    void menuChanged();
    void modeChanged();
    void statusChanged();
    void connectionStatusChanged();
private slots:
    void readData(const QString &);
    void onConnected();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    database::db_manager db;
    QWebSocket *m_socket = nullptr;
    BoardModel *m_current_board = nullptr;
    BoardMenu *m_board_menu = nullptr;
    QString m_username;
    QString m_password;
    quint32 m_user_id;
    quint32 m_local_id = 1;
    ConnectionStatus m_connection_status = ConnectionStatus::Unauthorized;
    int m_current_index = -1;

    void write(std::string &data);
    QString get_current_board_name();
    void load_remote_boards(const nlohmann::json &avaliable_boards = {});
    void unload_remote_boards();
};

#endif  // CLIENT_HPP
