#ifndef GWSEXTERNALCOMMUNICATOR_H
#define GWSEXTERNALCOMMUNICATOR_H

#include <QWebSocket>
#include <QObject>
#include <QString>

class GWSExternalCommunicator : public QObject
{
    Q_OBJECT

public:
    GWSExternalCommunicator( const QString &socket_id );

    enum CommunicatorStatus {
        NOT_STARTED = 0,
        DISCONNECTED = 1,
        CONNECTED = 2,
        SCHEDULED_PING = 3,
        PING_RECEIVED = 4
    };

public slots:

    void startSocket();
    void reconnectSocket();

protected:
    QString socket_id;
    QWebSocket websocket; // WS to sockets.geoworldsim.com
    CommunicatorStatus status;
};

#endif // GWSEXTERNALCOMMUNICATOR_H
