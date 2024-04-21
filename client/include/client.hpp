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
    Q_PROPERTY(ClientMode client_mode MEMBER m_mode NOTIFY modeChanged)
    Q_PROPERTY(ConnectionStatus connection_status MEMBER m_connection_status
                   NOTIFY connectionStatusChanged)
    Q_PROPERTY(bool is_authorized MEMBER m_is_authorized NOTIFY statusChanged)
    Q_PROPERTY(QString current_board_name READ get_current_board_name NOTIFY boardChanged)

public:
    enum ConnectionStatus {
        Unauthorized,
        Authentification_failed,
        Waiting_for_response,
        Unable_to_connect,
        Authorized
    };
    Q_ENUMS(ConnectionStatus)

    enum ClientMode { Local, Remote };
    Q_ENUMS(ClientMode)

    explicit Client(QObject *parent = nullptr);

    Q_INVOKABLE void create_board(QString name, QString description);
    Q_INVOKABLE void login(
        QString username,
        QString password,
        QString server_ip,
        QString server_port
    );
    Q_INVOKABLE void create_list(QString name);
    Q_INVOKABLE void
    create_card(int list_index, QString name, QString description);
    Q_INVOKABLE void prepare_local_board_select_menu();
    Q_INVOKABLE void prepare_remote_board_select_menu(
        const nlohmann::json &avaliable_boards = {}
    );
    Q_INVOKABLE void request_board(int index);
    Q_INVOKABLE void delete_list(int list_index);
    Q_INVOKABLE void delete_card(int list_index, int card_index);
    Q_INVOKABLE void
    update_card_name(int list_index, int card_index, QString name);
    Q_INVOKABLE void update_card_description(
        int list_index,
        int card_index,
        QString description
    );
    Q_INVOKABLE void update_list_name(int list_index, QString name);

signals:
    void boardChanged();
    void menuChanged();
    void modeChanged();
    void statusChanged();
    void connectionStatusChanged();
private slots:
    void readData(const QString &);
    void load_board(int index);
    void onConnected();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    database::db_manager db;
    QWebSocket *m_socket = nullptr;
    BoardModel *m_current_board = nullptr;
    QMap<int, BoardModel *> m_loaded_boards;
    BoardMenu *m_remote_menu = nullptr;
    BoardMenu *m_local_menu = nullptr;
    BoardMenu *m_board_menu = nullptr;
    QString m_username;
    QString m_password;
    QString m_server_ip;
    quint32 m_server_port;
    quint32 m_user_id;
    quint32 m_local_id = 1;
    quint32 m_local_group_id = 18;
    bool m_is_authorized = false;
    ConnectionStatus m_connection_status = ConnectionStatus::Unauthorized;
    ClientMode m_mode = ClientMode::Local;
    int m_current_index = -1;

    void write(std::string &data);
    QString get_current_board_name();
};

#endif  // CLIENT_HPP
