#include "ExternalCommunicator.h"

#include <QTimer>
#include <QDebug>

GWSExternalCommunicator::GWSExternalCommunicator( QString socket_id ) : QObject (){
    this->socket_id = socket_id;
    this->startSocket();
}

void GWSExternalCommunicator::startSocket(){

    // Connect and send info
    QObject::connect( &this->websocket , &QWebSocket::connected , [this](){
        qInfo() << QString("Websocket %1 connected successfully to %2").arg( this->socket_id ).arg( this->websocket.peerAddress().toString() );
    });

    // Keep alive
    QObject::connect( &this->websocket , &QWebSocket::pong , [this](quint64 elapsedTime, const QByteArray &payload){
        Q_UNUSED(elapsedTime); Q_UNUSED(payload);
        emit this->websocket.ping();
    });

    // Events
    QObject::connect( &this->websocket , &QWebSocket::disconnected , [this](){
        QTimer::singleShot( 10*1000 , this , &GWSExternalCommunicator::reconnectSocket );
    });

    this->reconnectSocket();
}

void GWSExternalCommunicator::reconnectSocket(){
    this->websocket.open( QUrl( "ws://sockets.geoworldsim.com/?scenario_id=" + this->socket_id ) );
}
