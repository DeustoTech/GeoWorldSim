#ifndef GWSEXTERNALLISTENER_H
#define GWSEXTERNALLISTENER_H

#include <QWebSocket>
#include <QObject>
#include <QString>

class GWSExternalListener : public QObject
{
    Q_OBJECT

public:
    GWSExternalListener( QString simulation_id );

public slots:

    void startSocket();
    void reconnectSocket();
    void messageReceived(const QString message);

private:
    QString listening_simulation_id;
    QWebSocket websocket; // WS to sockets.geoworldsim.com
    bool reconnecting = false; // Flag
};

#endif // GWSEXTERNALLISTENER_H
