#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QObject>
#include <QTcpSocket>
#include "boardmodel.hpp"

class Client : public QObject {
    Q_OBJECT
    Q_PROPERTY(BoardModel *current_board MEMBER m_current_board NOTIFY
                   boardChanged)
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

    explicit Client(QObject *parent = nullptr);
    Q_INVOKABLE void create_board();
    Q_INVOKABLE void login(
        QString &username,
        QString &password,
        QString &server_ip,
        QString &server_port
    );
    void write(const QString &data);
    void parse_response(const QString &data);
    Q_INVOKABLE void create_list(QString& name);
    Q_INVOKABLE void create_card(int list_index, QString& name, QString& description);

signals:
    void boardChanged();
    void statusChanged();
private slots:
    void readData();

private:
    QTcpSocket *m_socket = nullptr;
    BoardModel *m_current_board = nullptr;
    ClientStatus m_status = ClientStatus::local_mode;
    QString m_server_ip;
    quint16 m_server_port;
    quint16 m_user_id;
};

#endif  // CLIENT_HPP
