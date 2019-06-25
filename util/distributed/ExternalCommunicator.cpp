#include "ExternalCommunicator.h"

#include <QDebug>
#include <QTimer>

#include "../../app/App.h"
#include "../api/APIDriver.h"

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
    // Make Scenario to listen
    GWSAPIDriver::globalInstance()->GET(
                QString("https://history.geoworldsim.com/api/scenario/%1/socket").arg( GWSApp::globalInstance()->getAppId() ) ,
                []( QNetworkReply* reply ){
                   reply->connect( reply , &QNetworkReply::finished , reply , &QNetworkReply::deleteLater );
    });
    QTimer::singleShot( 30000 , [this](){ this->websocket.ping(); });
}
