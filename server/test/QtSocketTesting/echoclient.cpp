// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "echoclient.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtWebSockets/QWebSocket>
#include <QWebSocketHandshakeOptions>

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

    connect(&m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
            this, &EchoClient::onSslErrors);

    // QSslConfiguration sslConfiguration;
    // QFile certFile(QStringLiteral(":/localhost.cert"));
    // certFile.open(QIODevice::ReadOnly);
    // QFile privateFile(QStringLiteral(":/localhost.key"));
    // privateFile.open(QIODevice::ReadOnly);

    // QSslKey key()
    // QSslCertificate certificate(&certFile, QSsl::Pem);
    // certFile.close();
    // sslConfiguration.addCaCertificate(certificate);
    // m_webSocket.setSslConfiguration(sslConfiguration);

    qDebug() << "Readed";
    m_webSocket.open(url);
    m_webSocket.continueInterruptedHandshake();
    qDebug() << m_webSocket.errorString();
    // qDebug() << 
}
//! [constructor]

void EchoClient::onSslErrors(const QList<QSslError> &errors) {
    qDebug() << "Errors: " << errors;
    m_webSocket.ignoreSslErrors(errors);
}


void EchoClient::sendData(const QString &data) {
    m_webSocket.sendTextMessage(data);
}

EchoClient::~EchoClient() {
    m_webSocket.close();
}
//! [onConnected]
void EchoClient::onConnected() {
    qDebug() << "Connected";
    sendData("{ \"type\": \"login\", \"password\": \"1\", \"username\": \"user 4\"}");
    sendData(R"({"type":"upload", "object-json": {"type":"board","lists":[],"id":1,"name":12,"description":"this is board 3","link":""}})");
    // sendData(R"({"type":"move","board-id":1,"list-id":3,"card-id":4,"tag-id":0,"old-list-id":3,"new-list-id":4,"new-index":0})");
    // sendData(R"({"type": "update", "board-id": 1, "list-id": 2, "card-id": 3, "tag-id": 0, "id": 1, "new-value": "aboba", "field": "name", "object-type": "board"})");
    // sendData(R"({"type": "create", "board-id": 0, "list-id": 0, "card-id": 0, "tag-id": 0, "parent-id": 3, "object-type": "board", "name": "board 4", "description": "this is board 4"})");
    // sendData("{ \"type\": \"get-boards-info\", \"id\": 1, \"object-type\": \"list\", \"name\": \"52\", \"description\": \"42\"}");
}
//! [onConnected]

//! [onTextMessageReceived]
void EchoClient::onTextMessageReceived(QString message) {
    qDebug() << "Message received:" << message;
}
//! [onTextMessageReceived]
