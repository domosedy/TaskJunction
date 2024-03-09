#ifndef SOCKET_HANDLER_HPP_
#define SOCKET_HANDLER_HPP_

#include <QByteArray>
#include <QObject>
#include <QTcpSocket>

class ClientSocket : public QObject {
    Q_OBJECT

public:
    ClientSocket(QObject *parent);
    ~ClientSocket();
    ClientSocket(const ClientSocket &) = delete;
    ClientSocket &operator=(const ClientSocket &) = delete;

public slots:
    void sendData(const QByteArray &data);
    void readData();
    void removeConnection();

signals:
    void disconnected();

private:
    QTcpSocket *m_socket;
};

#endif