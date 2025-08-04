#include "Transfor.h"

Transfor* Transfor::_instance = nullptr;

Transfor *Transfor::global(QObject* parent) {
    if (!_instance) {
        _instance = new Transfor(parent);
    }
    return _instance;
}

void Transfor::setConnection(const QHostAddress &address, const quint32 port, const quint32 time_out) {
    _host_address = address;
    _host_port = port;
    _time_out = time_out;
}

void Transfor::setFile(const QString& file_name) {
    _file_list.clear();
    _file_list.append(file_name);
}

void Transfor::setFiles(const QList<QString>& file_list) {
    _file_list.clear();
    _file_list = file_list;
}

void Transfor::setDir(const QString& dir_name) {
    _dir_list.clear();
    _dir_list.append(dir_name);
}

void Transfor::setDirs(const QList<QString> &dir_list) {
    _dir_list.clear();
    _dir_list = dir_list;
}

const QString Transfor::hostAddress() const {
    return _host_address.toString();
}

const quint32 &Transfor::hostPort() const {
    return _host_port;
}

const QList<QString> &Transfor::fileList() const {
    return _file_list;
}

const QList<QString> &Transfor::dirList() const {
    return _dir_list;
}

void Transfor::start() {
    _tcp_socket->connectToHost(_host_address, _host_port);
    _tcp_socket->waitForConnected(_time_out);
    _tcp_socket->waitForReadyRead(_time_out);
}

void Transfor::stop() {
    _tcp_socket->disconnectFromHost();
}

void Transfor::transfor() {
    if (!_tcp_socket->isValid()) {
        qDebug() << "Can't send the message!";
        return;
    }
    if (_dir_list.empty() && _file_list.empty()) {
        _tcp_socket->write("Nothing to transfor!");
    } else {
        _tcp_socket->write("\nTransfor files: ");
        for (auto& f : _file_list) {
            _tcp_socket->write(QString("\n%1").arg(f).toStdString().c_str());
        }
        for (auto& d : _dir_list) {
            _tcp_socket->write(QString("\n%1").arg(d).toStdString().c_str());
        }
        _tcp_socket->write("\n");
    }
}

void Transfor::readyRead() {
    auto ctxt = _tcp_socket->readAll();
    emit received(QString::fromUtf8(ctxt));
}

void Transfor::errorOccured(QAbstractSocket::SocketError error) {
    emit connectionError(error);
}

Transfor::~Transfor() {
}

Transfor::Transfor(QObject *parent)
    : QObject{parent} {
    _tcp_socket = new QTcpSocket(parent);
    connect(_tcp_socket, &QTcpSocket::connected, [&] {
        _is_connected = true;
        _tcp_socket->waitForBytesWritten(_time_out);
        emit connected();
    });
    connect(_tcp_socket, &QTcpSocket::disconnected, [&] {
        _is_connected = false;
        emit disconnected();
    });
    connect(_tcp_socket, &QTcpSocket::errorOccurred, this, &Transfor::errorOccured);
    connect(_tcp_socket, &QTcpSocket::readyRead, this, &Transfor::readyRead);
}
