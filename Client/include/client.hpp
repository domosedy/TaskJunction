#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QMap>
#include <QObject>
#include <QTcpSocket>
#include <nlohmann/json.hpp>
#include <string>
#include "boardmenu.hpp"
#include "boardmodel.hpp"

class Client : public QObject {
    Q_OBJECT
    Q_PROPERTY(BoardModel *current_board MEMBER m_current_board NOTIFY
                   boardChanged)
    Q_PROPERTY(BoardMenu *board_menu MEMBER m_board_menu NOTIFY menuChanged)
    Q_PROPERTY(ClientStatus client_status MEMBER m_status NOTIFY statusChanged)

public:
    enum ClientStatus {
        local_mode,
        connected_to_server,
        authentification_failed,
        waiting_for_response,
        unable_to_connect
    };
    Q_ENUM(ClientStatus)

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
    void create_card(quint16 list_id, Card &new_Card);
private slots:
    void readData();
    void request_board(int index);

private:
    QTcpSocket *m_socket = nullptr;
    BoardModel *m_current_board = nullptr;
    QMap<quint16, BoardModel *> loaded_boards;
    BoardMenu *m_board_menu = nullptr;
    ClientStatus m_status = ClientStatus::local_mode;
    QString m_server_ip;
    quint16 m_server_port;
    quint16 m_user_id;

    void write(std::string &data);
    void parse_response(const QString &data);
    void create_board_select_menu(const nlohmann::json &avaliable_boards);
};

#endif  // CLIENT_HPP
