#ifndef EXTERNALCOMMUNICATOR_H
#define EXTERNALCOMMUNICATOR_H

#include <QWebSocket>
#include <QObject>
#include <QString>

class GWSExternalCommunicator : public QObject
{
    Q_OBJECT

public:
    GWSExternalCommunicator( QString socket_id );

public slots:

    void startSocket();
    void reconnectSocket();

protected:
    QString socket_id;
    QWebSocket websocket; // WS to sockets.geoworldsim.com
    bool reconnecting = false; // Flag

};

#endif // EXTERNALCOMMUNICATOR_H
