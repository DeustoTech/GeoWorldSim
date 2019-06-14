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

public slots:

    void startSocket();
    void reconnectSocket();

protected:
    QString socket_id;
    QWebSocket websocket; // WS to sockets.geoworldsim.com
    QString last_status;
};

#endif // GWSEXTERNALCOMMUNICATOR_H
