#ifndef CLI_H
#define CLI_H
#include "Server.h"
#include "Transfor.h"

struct TransforConfig {
    QHostAddress addr;
    quint32 port;
    QList<QString> file_list;
    QList<QString> dir_list;
};

static bool is_server = false;
void stopAll();
void prepareFiles(const QList<QString> file_list);
void prepareDirs(const QList<QString> dir_list);

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
    QObject::connect(server, &Server::received, [] (const QString& uuid, const QString& msg) {
        qInfo().nospace().noquote() << "Received msg: " << msg << " (From " << uuid << ")";
    });
    is_server = true;
    server->setPort(port);
    server->start();
}

void startClient(TransforConfig config) {
    Transfor* transfor = Transfor::global();
    QObject::connect(transfor, &Transfor::connected, [&] {
        qInfo().nospace().noquote() << "Connection successful!";
        if (!config.file_list.empty()) {
            prepareFiles(config.file_list);
        }
        if (!config.dir_list.empty()) {
            prepareDirs(config.dir_list);
        }
        transfor->transfor();
    });
    QObject::connect(transfor, &Transfor::disconnected, [&] {
        qInfo() << "Disconnected from server. Bye~";
    });
    QObject::connect(transfor, &Transfor::connectionError, [&] (const QTcpSocket::SocketError& e) {
        qCritical().nospace().noquote() << "Connection Error! (Error: " << e << ")";
        stopAll();
        exit(8);
    });
    QObject::connect(transfor, &Transfor::received, [] (const QString& msg) {
        qInfo() << "Received:" << msg;
    });
    transfor->setConnection(config.addr, config.port, 10000);
    is_server = false;
    qInfo().nospace().noquote() << "Connecting to server " << transfor->hostAddress() << ":" << transfor->hostPort() << "...";
    transfor->start();
}

void prepareFiles(const QList<QString> file_list) {
    Transfor* transfor = Transfor::global();
    transfor->setFiles(file_list);
}

void prepareDirs(const QList<QString> dir_list) {
    Transfor* transfor = Transfor::global();
    transfor->setDirs(dir_list);
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
