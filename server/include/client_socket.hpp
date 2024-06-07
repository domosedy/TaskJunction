#ifndef CLIENT_SOCKET_HPP_
#define CLIENT_SOCKET_HPP_

#include "logging.hpp"
#include "query.hpp"
#include <QByteArray>
#include <QObject>
#include <QTcpSocket>
#include <QWebSocket>
#include <memory>

class ClientSocket : public QObject {
  Q_OBJECT

private:
  QWebSocket *socket;
  uint user_id;

public:
  ClientSocket(QWebSocket *sock, uint user_id)
      : socket(sock), user_id(user_id) {
    connect(socket, SIGNAL(textMessageReceived(const QString &)), this,
            SLOT(readData(const QString &)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(removeConnection()));
  }

  ~ClientSocket() { socket->close(); }

  uint get_client_id() const noexcept { return user_id; }

  void set_client_id(uint new_id) noexcept { user_id = new_id; }

  ClientSocket(const ClientSocket &) = delete;
  ClientSocket &operator=(const ClientSocket &) = delete;

public slots:
  void sendData(const QString &data);
  void readData(const QString &data);
  void removeConnection();

signals:
  void request_to_database(uint user_id, const query_type &query);
  void disconnected();
};

#endif