#include "ExternalCommunicator.h"

#include <QDebug>
#include <QTimer>

GWSExternalCommunicator::GWSExternalCommunicator( const QString &socket_id ) : QObject (){
    this->socket_id = socket_id;
    this->startSocket();
}

void GWSExternalCommunicator::startSocket(){

    // Connect and send info
    QObject::connect( &this->websocket , &QWebSocket::connected , [this](){
        this->last_status = "connected";
        qInfo() << QString("Websocket %1 connected successfully to %2").arg( this->socket_id ).arg( this->websocket.peerAddress().toString() );
    });

    // Keep alive
    QObject::connect( &this->websocket , &QWebSocket::pong , [this](quint64 elapsedTime, const QByteArray &payload){
        Q_UNUSED(elapsedTime); Q_UNUSED(payload);
        this->last_status = "pong";
        QTimer::singleShot( 30000 , [this](){ this->websocket.ping(); });
    });

    // Rise from the grave
    QObject::connect( &this->websocket , &QWebSocket::disconnected , [this](){
        this->last_status = "disconnected";
        QTimer::singleShot( 1000 , this , &GWSExternalCommunicator::reconnectSocket );
    });

    this->reconnectSocket();
}

void GWSExternalCommunicator::reconnectSocket(){
    this->websocket.open( QUrl( "wss://sockets.geoworldsim.com/?scenario_id=" + this->socket_id ) );
    QTimer::singleShot( 30000 , [this](){ this->websocket.ping(); });
}
