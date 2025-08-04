#ifndef CLI_H
#define CLI_H
#include "Server.h"
#include "Transfor.h"
static bool is_server = false;
void stopAll();

void startServer(quint32 port) {
    Server* server = Server::global();
    QObject::connect(server, &Server::serverStarted, [&] {
        qInfo().nospace().noquote() << "Server " << server->hostAddress() << ":" << server->port() << " is started now!";
    });
    QObject::connect(server, &Server::serverStopped, [&] {
        qInfo() << "Server is stopped now!";
    });
    QObject::connect(server, &Server::newOneConnected, [] (const QString& uuid) {
        qInfo().nospace().noquote() << "A new connection is connected! " << "(UUID: " << uuid << ")";
    });
    QObject::connect(server, &Server::someOneDisconnected, [] (const QString& uuid) {
        qInfo().nospace().noquote() << "The connection is disconnected! " << "(UUID: " << uuid << ")";
    });
    is_server = true;
    server->setPort(port);
    server->start();
}

void startClient(const QHostAddress& addr, quint32 port) {
    Transfor* transfor = Transfor::global();
    QObject::connect(transfor, &Transfor::connected, [&] {
        qInfo().nospace().noquote() << "Connection successful!";
    });
    QObject::connect(transfor, &Transfor::disconnected, [&] {
        qInfo() << "Disconnected from server. Bye~";
    });
    QObject::connect(transfor, &Transfor::connectionError, [&] (const QTcpSocket::SocketError& e) {
        qCritical().nospace().noquote() << "Connection Error! (Error: " << e << ")";
        stopAll();
        exit(8);
    });
    QObject::connect(transfor, &Transfor::received, [&] (const QString& msg) {
        qInfo() << "Received:" << msg;
    });
    transfor->setConnection(addr, port);
    is_server = false;
    qInfo().nospace().noquote() << "Connecting to server " << transfor->hostAddress() << ":" << transfor->hostPort() << "...";
    transfor->start();
}

void stopAll() {
    if (!is_server) {
        Transfor* transfor = Transfor::global();
        transfor->stop();
        delete transfor;
    } else {
        Server* server = Server::global();
        server->stop();
        delete server;
    }
}

#endif // CLI_H
