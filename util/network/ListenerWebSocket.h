#ifndef LISTENERWEBSOCKET_H
#define LISTENERWEBSOCKET_H

#include "WebSocketDriver.h"

#include <QJsonObject>

namespace geoworldsim {
namespace network {


class ListenerWebSocket : public WebSocketDriver
{
    Q_OBJECT

public:
    ListenerWebSocket( QString socket_id );


signals:
    void dataReceivedSignal( QJsonObject json_data );

};

}
}


#endif // LISTENERWEBSOCKET_H
