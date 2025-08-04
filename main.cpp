#include <QApplication>
#include <QCommandLineParser>
#include <QString>
#include <QHostAddress>
#include <csignal>
#include "src/CLI.h"

int commandMode(const QCoreApplication& app) {
    QCommandLineOption op_server({"s", "server"}, "Run as server mode."),
        op_client({"c", "client"}, "Run as client mode"),
        op_host({"i", "ip"}, "Connect the server (for Client user)", "ip"),
        op_port({"p", "port"}, "Port number (Default: 8000)", "port", "8000"),
        op_file({"f", "file"}, "Send the file(s) to server (for Client user)", "filepath"),
        op_dir({"d", "directory"}, "Send the specified directory to server (for Client user)", "directory"),
        op_save_dir({"o", "output"}, "Output and save to the specifed directory (for Client user)", "directory"),
        op_config({"e", "config"}, "Load for config file (Only support for json)", "filepath");
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions(QList<QCommandLineOption>() << op_server << op_client
                                                  << op_host << op_port
                                                  << op_file << op_dir
                                                  << op_save_dir << op_config);
    parser.process(app);

    bool is_server = parser.isSet(op_server);
    bool is_client = parser.isSet(op_client);
    bool ok = false;
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
    if (is_server) {
        qSetMessagePattern("[%{time}] %{message}");
        startServer(port);
    } else if (is_client) {
        qSetMessagePattern("[%{time}] %{message}");
        startClient(addr, port);
    } else {
        qCritical() << "Error: You have not run as server or client mode! Please use '-s' or '-c' option.";
        return 1;
    }
    return 0;
}

void exit_anyway(int signal) {
    qCritical().nospace() << "Exit programme at code " << signal << ".";
    stopAll();
    exit(signal);
}

int main(int argc, char *argv[])
{
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
