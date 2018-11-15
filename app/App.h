#ifndef GWSAPP_H
#define GWSAPP_H

#include <QtGlobal>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QtWebSockets/QWebSocket>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class GWSApp : public QCoreApplication
{
    Q_OBJECT

public:
    static GWSApp* globalInstance(int argc = 0, char* argv[] = 0);

    // GETTERS
    QString getAppId();
    QString getUserId();

public slots:

    // METHODS
    int exec();

    // SEND DATA
    void startSocket();
    void reconnectSocket();

signals: // MUST BE USED THROUGH THESE SIGNALS
    void sendAgentToSocketSignal( QJsonObject agent_json );
    void sendAlertToSocketSignal( int level, QString title, QString description = QString() );
    void sendDataToSocketSignal( QString signal , QJsonValue json );

private slots:
    void sendAgentToSocket( QJsonObject agent_json );
    void sendAlertToSocket( int level, QString title, QString description );
    void sendDataToSocket( QString signal , QJsonValue json );

private:
    GWSApp(int argc, char* argv[]);
    GWSApp(GWSApp const&);
    ~GWSApp();

    QWebSocket websocket; // WS to sockets.geoworldsim.com
    QNetworkAccessManager http_manager; // HTTP Posts to alerts.geoworldsim.com
    quint64 created_timestamp;

};

#endif // GWSAPP_H
