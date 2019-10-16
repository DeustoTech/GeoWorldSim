#ifndef PUBLISHERWEBSOCKET_H
#define PUBLISHERWEBSOCKET_H

#include <QJsonObject>

#include "WebSocketDriver.h"

namespace geoworldsim {
namespace network {


class PublisherWebSocket : public WebSocketDriver
{
Q_OBJECT

public:
    PublisherWebSocket(const QString &socket_id);

    void sendMessage(const QString &signal , const QJsonValue &json);

};

}
}

#endif // PUBLISHERWEBSOCKET_H
