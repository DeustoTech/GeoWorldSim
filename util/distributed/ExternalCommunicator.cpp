#include "ExternalCommunicator.h"

#include <QDebug>
#include <QTimer>

#include "../../app/App.h"
#include "../api/APIDriver.h"

GWSExternalCommunicator::GWSExternalCommunicator( const QString &socket_id ) : QObject (){
    this->socket_id = socket_id;
    this->status = NOT_STARTED;
    this->startSocket();
}

void GWSExternalCommunicator::startSocket(){

    // Connect and send info
    QObject::connect( &this->websocket , &QWebSocket::connected , [this](){
        qInfo() << QString("Websocket %1 connected successfully to %2, coming from status %3").arg( this->socket_id ).arg( this->websocket.peerAddress().toString() ).arg( this->status );
        this->status = CONNECTED;
    });

    // Keep alive
    QObject::connect( &this->websocket , &QWebSocket::pong , [this](quint64 elapsedTime, const QByteArray &payload){
        Q_UNUSED(elapsedTime); Q_UNUSED(payload);

        if( this->status == SCHEDULED_PING ){ return; }

        this->status = SCHEDULED_PING;
        QTimer::singleShot( 30000 , [this](){
            this->status = PING_RECEIVED;
            this->websocket.ping();
        });
    });

    // Rise from the grave
    QObject::connect( &this->websocket , &QWebSocket::disconnected , [this](){
        this->websocket.close();
        this->status = DISCONNECTED;
        QTimer::singleShot( 1000 , this , &GWSExternalCommunicator::reconnectSocket );
    });

    this->reconnectSocket();
}

void GWSExternalCommunicator::reconnectSocket(){

    this->websocket.open( "wss://sockets.geoworldsim.com/?scenario_id=" + this->socket_id );

    // Make Scenario to listen
    GWSAPIDriver::globalInstance()->GET(
                QString("https://history.geoworldsim.com/api/scenario/%1/socket").arg( GWSApp::globalInstance()->getAppId() ) ,
                []( QNetworkReply* reply ){
                   reply->connect( reply , &QNetworkReply::finished , reply , &QNetworkReply::deleteLater );
    });

    if( this->status == SCHEDULED_PING ){ return; }

    this->status = SCHEDULED_PING ;
    QTimer::singleShot( 30000 , [this](){
        this->status = PING_RECEIVED;
        this->websocket.ping();
    });
}
