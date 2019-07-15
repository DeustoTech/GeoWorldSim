#include "ExternalCommunicator.h"

#include <QDebug>
#include <QTimer>

#include "../../app/App.h"
#include "../api/APIDriver.h"

GWSExternalCommunicator::GWSExternalCommunicator( const QString &socket_id ) : QObject (){
    this->socket_id = socket_id;
    this->communicator_status = NOT_STARTED;
    this->startSocket();
}

void GWSExternalCommunicator::startSocket(){

    // Connect and send info
    QObject::connect( &this->websocket , &QWebSocket::connected , [this](){
        qInfo() << QString("Websocket %1 connected successfully to %2, coming from status %3").arg( this->socket_id ).arg( this->websocket.peerAddress().toString() ).arg( this->communicator_status );

        this->communicator_status = CONNECTED;

        // Make Scenario to listen
        GWSAPIDriver::globalInstance()->GET(
                    QString("https://history.geoworldsim.com/api/scenario/%1/socket?user_id=%2").arg( GWSApp::globalInstance()->getAppId() ).arg( GWSApp::globalInstance()->getConfiguration().value("user_id").toString() ) ,
                    []( QNetworkReply* reply ){
                       reply->connect( reply , &QNetworkReply::finished , reply , &QNetworkReply::deleteLater );
        });

        this->communicator_status = PING_SCHEDULED;
        QTimer::singleShot( 30000 , [this](){

            this->mutext.lockForWrite();
            this->communicator_status = PING_SENT;
            this->websocket.ping();
            this->mutext.unlock();

        });

    });

    // Keep alive
    QObject::connect( &this->websocket , &QWebSocket::pong , [this](quint64 elapsedTime, const QByteArray &payload){
        Q_UNUSED(elapsedTime); Q_UNUSED(payload);

        if( this->communicator_status == PING_SCHEDULED ){ return; }

        this->communicator_status = PING_SCHEDULED;
        QTimer::singleShot( 30000 , [this](){

            this->mutext.lockForWrite();
            if( this->websocket.state() == QAbstractSocket::ConnectedState ){
                this->communicator_status = PING_SENT;
                this->websocket.ping();
            } else {
                this->websocket.close();
                this->communicator_status = DISCONNECTED;
                QTimer::singleShot( 1000 , this , &GWSExternalCommunicator::reconnectSocket );
            }
            this->mutext.unlock();

        });
    });

    // Rise from the grave
    QObject::connect( &this->websocket , &QWebSocket::disconnected , [this](){

        this->mutext.lockForWrite();
        this->websocket.close();
        this->communicator_status = DISCONNECTED;
        QTimer::singleShot( 1000 , this , &GWSExternalCommunicator::reconnectSocket );
        this->mutext.unlock();
    });

    this->reconnectSocket();
}

void GWSExternalCommunicator::reconnectSocket(){
    this->websocket.open( "wss://sockets.geoworldsim.com/?scenario_id=" + this->socket_id );
}
