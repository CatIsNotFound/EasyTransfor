#include <QApplication>
#include <QCommandLineParser>
#include <QString>
#include <QDateTime>
#include <QHostAddress>
#include <QMutex>
#include <QFile>
#include <QDir>
#include <csignal>
#include <iostream>
#include "src/CLI.h"

// Global variables
static QString _log_file_path;
static bool _log_trigger;

void writeLogFile(QtMsgType type, const QMessageLogContext& ctxt, const QString& str) {
    static QMutex mutex;
    mutex.lock();
    QString msg, m_type;
    QString datetime = QDateTime::currentDateTime().toString();
    QString log_file_name = QDateTime::currentDateTime().toString("yyyymmddhhmmss");
    switch (type) {
    case QtDebugMsg:
        m_type = "Debug";
        break;
    case QtInfoMsg:
        m_type = "Info";
        break;
    case QtWarningMsg:
        m_type = "Warning";
        break;
    case QtCriticalMsg:
        m_type = "Error";
        break;
    case QtFatalMsg:
        m_type = "Fatal";
        break;
    }

    msg = QString("[%1] [%2]: %3\n").arg(datetime, m_type, str);
    std::cout << msg.toLocal8Bit().data();
    if (_log_trigger) {
        QFile file;
        QDir dir;
        dir.mkdir(QString("%1/logs").arg(QApplication::applicationDirPath()));
        QString default_path = QString("%1/logs/%2.txt").arg(QApplication::applicationDirPath())
                                                        .arg(log_file_name);
        file.setFileName(_log_file_path.isEmpty() ? default_path : _log_file_path);
        _log_file_path = (_log_file_path.isEmpty() ? default_path : _log_file_path);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            file.write(msg.toLocal8Bit());
            file.close();
        }
    }
    mutex.unlock();
}

int commandMode(const QCoreApplication& app) {
    QCommandLineOption op_server({"s", "server"}, "Run as server mode."),
        op_client({"c", "client"}, "Run as client mode"),
        op_host({"i", "ip"}, "Connect the server (Only for Client user)", "ip"),
        op_port({"p", "port"}, "Port number (Default: 8000)", "port", "8000"),
        op_file({"f", "file"}, "Send the file(s) to server (Only for Client user)", "filepath"),
        op_dir({"d", "directory"}, "Send the specified directory to server (Only for Client user)", "directory"),
        op_save_dir({"o", "output"}, "Output and save to the specifed directory (Only support for Client user)", "directory"),
        op_config({"e", "config"}, "Load for config file (Only support for json)", "filepath"),
        op_no_log("no-log", "Don't write any logs");
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions(QList<QCommandLineOption>() << op_server << op_client
                                                  << op_host << op_port
                                                  << op_file << op_dir
                                                  << op_save_dir << op_config
                                                  << op_no_log);
    parser.process(app);

    bool is_server = parser.isSet(op_server);
    bool is_client = parser.isSet(op_client);
    bool ok = false;
    _log_trigger = !parser.isSet(op_no_log);
    quint32 port = parser.value(op_port).toUInt(&ok);
    if (!ok || port <= 1000 || port > 65535) {
        qCritical() << "Error: The port number is not valid! (Accept values: 1000-65535)";
        return 1;
    }
    QString host = parser.value(op_host);
    QHostAddress addr;
    if (!host.isEmpty()) {
        addr.setAddress(host);
        if (addr.isNull()) {
            qCritical() << "Error: The host address is not valid! (e.g: 127.0.0.1)";
            return 1;
        }
    }
    qInstallMessageHandler(writeLogFile);
    if (is_server) {
        startServer(port);
    } else if (is_client) {
        QList<QString> file_list;
        QList<QString> dir_list;
        if (parser.isSet(op_file)) {
            file_list << parser.values(op_file);
        }
        if (parser.isSet(op_dir)) {
            dir_list << parser.values(op_dir);
        }
        TransforConfig config = {addr, port, file_list, dir_list};
        startClient(config);

    } else {
        qCritical() << "Error: You have not run as server or client mode! Please use '-s' or '-c' option.";
        return 1;
    }
    return 0;
}

void exit_anyway(int signal) {
    qCritical().nospace() << "Exit programme at code " << signal << ".";
    if (_log_trigger) {
        qInfo().nospace().noquote() << "Note: You can check log file in "
                                    << _log_file_path << " for more details.";
        }
    stopAll();
    exit(signal);
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QApplication::setApplicationName("EasyTransfor");
    QApplication::setApplicationDisplayName("Easy Transfor");
    QApplication::setApplicationVersion("v0.1.1-beta");

    if (argc < 2) {
        // TODO: Display the graphic mode.

    } else {
        signal(SIGINT, exit_anyway);
        signal(SIGTERM, exit_anyway);
        int ret = commandMode(a);
        if (ret) return ret;
    }

    return a.exec();
}
