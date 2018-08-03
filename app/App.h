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

public slots:

    // METHODS
    int exec();

    // SEND DATA
    void startSocket();
    void reconnectSocket();

signals:
    void pushAgentSignal( QJsonObject agent_json );
    void pushDataSignal( QString signal , QJsonValue json );

private slots:
    void pushAgent( QJsonObject entity_json );
    void pushData( QString signal , QJsonValue json );

private:
    GWSApp(int argc, char* argv[]);
    GWSApp(GWSApp const&);
    ~GWSApp();

    QString app_id; // Given by args
    QWebSocket websocket; // WS to sockets.deusto.io
    QNetworkAccessManager http_manager; // HTTP Posts to history.deusto.io
    quint64 created_timestamp;

};

#endif // GWSAPP_H
