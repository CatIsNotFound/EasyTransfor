#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPointer>

class Server : public QObject
{
    Q_OBJECT

public:
    static Server* global(QObject* parent = nullptr);
    void setPort(quint32 port);
    quint32 port() const;
    QString hostAddress() const;
    void start();
    void stop();
    void newConnection();
    void socketDisconnect();
    void readyRead(QTcpSocket* socket);

    ~Server();
signals:
    void serverStarted();
    void serverStopped();
    void newOneConnected(const QString& uuid);
    void someOneDisconnected(const QString& uuid);
    void received(const QString& msg);

private:
    explicit Server(QObject *parent = nullptr);

private:
    static Server* _instance;
    QPointer<QTcpServer> _server;
    QList<std::shared_ptr<QTcpSocket>> _socket_list;
    quint32 _port;

};

#endif // SERVER_H
