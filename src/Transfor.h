#ifndef TRANSFOR_H
#define TRANSFOR_H

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QString>
#include <QList>
#include <QFile>
#include <QDir>
#include <QPointer>
#include <QTimer>

class Transfor : public QObject
{
    Q_OBJECT
public:
    static Transfor* global(QObject* parent = nullptr);
    void setConnection(const QHostAddress& address, const quint32 port, const quint32 time_out = 5000);
    void setFile(const QString& file_name);
    void setFiles(const QList<QString>& file_list);
    void setDir(const QString& dir_name);
    void setDirs(const QList<QString>& dir_list);
    const QString hostAddress() const;
    const quint32& hostPort() const;
    const QList<QString>& fileList() const;
    const QList<QString>& dirList() const;

    void start();
    void stop();

    void readyRead();
    void errorOccured(QTcpSocket::SocketError error);
    ~Transfor();

private:
    explicit Transfor(QObject *parent = nullptr);

signals:
    void connected();
    void disconnected();
    void connectionError(QTcpSocket::SocketError& e);
    void received(const QString& msg);

private:
    static Transfor* _instance;
    QHostAddress _host_address;
    quint32 _host_port;
    quint32 _time_out{0};
    QList<QString> _file_list;
    QList<QString> _dir_list;
    QPointer<QTcpSocket> _tcp_socket;
    bool _is_connected{false};
};

#endif // TRANSFOR_H
