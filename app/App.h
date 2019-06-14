#ifndef GWSAPP_H
#define GWSAPP_H

#include <QtGlobal>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QtWebSockets/QWebSocket>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class GWSApp : public QCoreApplication
{
    Q_OBJECT

public:
    static GWSApp* globalInstance(int argc = 0, char* argv[] = 0);

    enum exit_codes {
    };

    // GETTERS
    QString getAppId();
    QString getUserId();
    QJsonObject getConfiguration();

public slots:

    // METHODS
    int exec();
    void exit(int retcode);

private:
    GWSApp(int argc, char* argv[]);
    GWSApp(GWSApp const&);
    ~GWSApp();

    QNetworkAccessManager http_manager; // HTTP Posts to alerts.geoworldsim.com

    // Configuration provided by arguments
    QJsonObject json_configuration;

    // Times
    const quint64 created_timestamp;
    quint64 load_time; // From creation time to start time
    quint64 started_timestamp;
    quint64 run_time; // From start to end

};

#endif // GWSAPP_H
