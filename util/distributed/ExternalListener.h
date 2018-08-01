#ifndef GWSEXTERNALLISTENER_H
#define GWSEXTERNALLISTENER_H

#include <QWebSocket>

#include "../../object/Object.h"

class GWSExternalListener : public GWSObject
{
    Q_OBJECT

public:
    GWSExternalListener( QString simulation_id );

public slots:

    void startSocket();
    void messageReceived(const QString message);

private:
    QString listening_simulation_id;
    QWebSocket websocket; // WS to sockets.deusto.io

};

#endif // GWSEXTERNALLISTENER_H
