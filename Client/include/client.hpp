#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QObject>
#include <QTcpSocket>
#include "boardmodel.hpp"

class Client : public QObject {
    Q_OBJECT
    Q_PROPERTY(BoardModel* current_board MEMBER m_current_board NOTIFY boardChanged)

public:
    enum LoginStatus {
        success,
        invalid_server,
        invalid_user,
        same_server
    };
    Q_ENUM(LoginStatus)

    explicit Client(QObject *parent = nullptr);
    Q_INVOKABLE void create_board();
    Q_INVOKABLE LoginStatus login(QString& username, QString& password, QString& server_ip, QString& server_port);
    void write(const QString& data);
    void parse_response(const QString& data);

signals:
    void boardChanged();
private slots:
    void readData();
private:
    QTcpSocket *m_socket = nullptr;
    BoardModel*  m_current_board = nullptr;
    QString m_server_ip;
    quint16 m_server_port;
    QString m_username;
    QString m_user_id;
};

#endif  // CLIENT_HPP
