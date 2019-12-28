#ifndef WEBSOCKETDRIVER_H
#define WEBSOCKETDRIVER_H

#include <QWebSocket>
#include <QObject>
#include <QString>
#include <QReadWriteLock>

namespace geoworldsim {
namespace network {


class WebSocketDriver : public QObject
{
    Q_OBJECT

public:
    WebSocketDriver( const QString &socket_id );

    enum SocketStatus {
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
    SocketStatus socket_status;

    mutable QReadWriteLock mutext;
};


}
}

#endif // WEBSOCKETDRIVER_H
