// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "echoclient.h"
#include <QtCore/QDebug>

QT_USE_NAMESPACE

//! [constructor]
EchoClient::EchoClient(const QUrl &url, bool debug, QObject *parent) :
    QObject(parent),
    m_debug(debug)
{
    if (m_debug)
        qDebug() << "WebSocket server:" << url;
    connect(&m_webSocket, &QWebSocket::connected, this, &EchoClient::onConnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &EchoClient::onTextMessageReceived);
    // connect(&m_webSocket, &QWebSocket::disconnected, this, &EchoClient::closed);
    m_webSocket.open(url);
}
//! [constructor]

void EchoClient::sendData(const QString &data) {
    m_webSocket.sendTextMessage(data);
}

EchoClient::~EchoClient() {
    m_webSocket.close();
}
//! [onConnected]
void EchoClient::onConnected() {
    sendData("{ \"type\": \"login\", \"password\": \"12\", \"user-name\": \"asss\"}");
    sendData(R"({"type": "update", "board-id": 4, "list-id": 2, "card-id": 3, "id": 2, "new-value": "aboba", "field": "name", "object-type": "board"})");
    sendData("{ \"type\": \"get-boards-info\", \"id\": 4, \"object-type\": \"list\", \"name\": \"52\", \"description\": \"42\"}");
}
//! [onConnected]

//! [onTextMessageReceived]
void EchoClient::onTextMessageReceived(QString message) {
    qDebug() << "Message received:" << message;
}
//! [onTextMessageReceived]
