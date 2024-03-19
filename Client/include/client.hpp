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
    Q_PROPERTY(ClientStatus client_status MEMBER m_status NOTIFY statusChanged)

public:
    enum ClientStatus {
        Local_mode,
        Connected_to_server,
        Authentification_failed,
        Waiting_for_response,
        Unable_to_connect
    };
    Q_ENUMS(ClientStatus)
    enum BoardType { local, remote };

    explicit Client(QObject *parent = nullptr);

    Q_INVOKABLE void create_board();
    Q_INVOKABLE void login(
        QString &username,
        QString &password,
        QString &server_ip,
        QString &server_port
    );
    Q_INVOKABLE void create_list(QString &name);
    Q_INVOKABLE void
    create_card(int list_index, QString &name, QString &description);

signals:
    void boardChanged();
    void menuChanged();
    void statusChanged();
    void create_card(quint32 list_id, card &new_Card);
private slots:
    void readData();
    void request_board(int index);

private:
    database::db_manager db;
    QTcpSocket *m_socket = nullptr;
    BoardModel *m_current_board = nullptr;
    QMap<quint32, BoardModel *> loaded_boards;
    BoardMenu *m_board_menu = nullptr;
    ClientStatus m_status = ClientStatus::Local_mode;
    QString m_server_ip;
    quint32 m_server_port;
    quint32 m_user_id;

    void write(std::string &data);
    void parse_response(const QString &data);
    void create_board_select_menu(const nlohmann::json &avaliable_boards);
};

#endif  // CLIENT_HPP
