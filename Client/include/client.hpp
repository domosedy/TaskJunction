#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QMap>
#include <QObject>
#include <QTcpSocket>
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

    Q_INVOKABLE void create_board(QString &name, QString &description);
    Q_INVOKABLE void login(
        QString &username,
        QString &password,
        QString &server_ip,
        QString &server_port
    );
    Q_INVOKABLE void create_list(QString &name);
    Q_INVOKABLE void
    create_card(int list_index, QString &name, QString &description);
    Q_INVOKABLE void prepare_local_board_select_menu();
    Q_INVOKABLE void prepare_remote_board_select_menu(
        const nlohmann::json &avaliable_boards = {}
    );
    Q_INVOKABLE void request_board(int index);
    Q_INVOKABLE void delete_list(int list_index);
    Q_INVOKABLE void delete_card(int list_index, int card_index);

signals:
    void boardChanged();
    void menuChanged();
    void modeChanged();
    void statusChanged();
    void connectionStatusChanged();
private slots:
    void readData();
    void load_board(int index);

private:
    database::db_manager db;
    QTcpSocket *m_socket = nullptr;
    BoardModel *m_current_board = nullptr;
    QMap<quint32, BoardModel *> m_loaded_boards;
    BoardMenu *m_remote_menu = nullptr;
    BoardMenu *m_local_menu = nullptr;
    BoardMenu *m_board_menu = nullptr;
    QString m_server_ip;
    quint32 m_server_port;
    quint32 m_user_id;
    quint32 m_local_id = 1;
    bool m_is_authorized = false;
    ConnectionStatus m_connection_status = ConnectionStatus::Unauthorized;
    ClientMode m_mode = ClientMode::Local;

    void write(std::string &data);
    void parse_response(const QString &data);
};

#endif  // CLIENT_HPP
