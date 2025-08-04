#include <QUuid>
#include "Server.h"

Server* Server::_instance = nullptr;

Server *Server::global(QObject *parent) {
    if (!_instance) {
        _instance = new Server(parent);
    }
    return _instance;
}

void Server::setPort(quint32 port) {
    _port = port;
}

quint32 Server::port() const {
    return _port;
}

QString Server::hostAddress() const {
    return _server->serverAddress().toString();
}

void Server::start() {
    if (!_server->isListening()) {
        bool b = _server->listen(QHostAddress::LocalHost, _port);
        if (b) emit serverStarted();
    }
}

void Server::stop() {
    if (_server->isListening()) {
        emit serverStopped();
        _server->close();
    }
}

void Server::newConnection() {
    while (_server->hasPendingConnections()) {
        QTcpSocket* new_socket = _server->nextPendingConnection();
        QUuid uuid = QUuid::createUuid();
        _socket_list.append(std::shared_ptr<QTcpSocket>(new_socket));
        _socket_list.back()->setProperty("uuid", uuid.toString(QUuid::WithoutBraces));
        emit newOneConnected(uuid.toString(QUuid::WithoutBraces));
        qDebug() << "Connection count:" << _socket_list.count();
        connect(new_socket, &QTcpSocket::disconnected, this, &Server::socketDisconnect);
        connect(new_socket, &QTcpSocket::readyRead, this, &Server::readyRead);
    }
}

void Server::socketDisconnect() {
    for (auto& socket : _socket_list) {
        if (socket->state() != QTcpSocket::ConnectedState) {
            emit someOneDisconnected(socket->property("uuid").toString());
            _socket_list.removeOne(socket);
            break;
        }
    }
}

void Server::readyRead() {
    for (auto& socket : _socket_list) {
        if (socket->bytesAvailable()) {
            auto bytes = socket->readAll();
            emit received(socket->property("uuid").toString(), QString::fromUtf8(bytes));
            break;
        }
    }
}

Server::~Server() {
}

Server::Server(QObject *parent)
    : QObject{parent} {
    _server = new QTcpServer(parent);
    connect(_server, &QTcpServer::newConnection, this, &Server::newConnection);
    connect(_server, &QTcpServer::acceptError, this, &Server::socketDisconnect);
}
