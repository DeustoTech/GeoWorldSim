#ifndef GWSEXTERNALLISTENER_H
#define GWSEXTERNALLISTENER_H

#include "ExternalCommunicator.h"

class GWSExternalListener : public GWSExternalCommunicator
{
    Q_OBJECT

public:
    GWSExternalListener( QString socket_id );

public slots:

    void messageReceived(const QString message);

private:
    QString listening_simulation_id;
    QWebSocket websocket; // WS to sockets.geoworldsim.com
    bool reconnecting = false; // Flag
};

#endif // GWSEXTERNALLISTENER_H
