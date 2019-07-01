#ifndef GWSEXTERNALCOMMUNICATOR_H
#define GWSEXTERNALCOMMUNICATOR_H

#include <QWebSocket>
#include <QObject>
#include <QString>
#include <QReadWriteLock>

class GWSExternalCommunicator : public QObject
{
    Q_OBJECT

public:
    GWSExternalCommunicator( const QString &socket_id );

    enum CommunicatorStatus {
        NOT_STARTED = 0,
        DISCONNECTED = 1,
        CONNECTED = 2,
        PING_SCHEDULED = 3,
        PING_SENT = 4
    };

public slots:

    void startSocket();
    void reconnectSocket();

protected:
    QString socket_id;
    QWebSocket websocket; // WS to sockets.geoworldsim.com
    CommunicatorStatus communicator_status;

    mutable QReadWriteLock mutext;
};

#endif // GWSEXTERNALCOMMUNICATOR_H
