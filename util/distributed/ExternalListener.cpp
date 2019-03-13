#include "ExternalListener.h"

#include <QJsonDocument>

#include "../../app/App.h"

GWSExternalListener::GWSExternalListener( QString socket_id ) : GWSExternalCommunicator( socket_id ){

    // Listening to yourself would be a loop
    Q_ASSERT( socket_id != GWSApp::globalInstance()->getAppId() );

  this->websocket.connect( &this->websocket , &QWebSocket::textMessageReceived , [this](const QString message){
        qDebug() << "Received message" << message.toUtf8();
        QJsonObject json = QJsonDocument::fromJson( message.toUtf8() ).object();
        json.insert( "scenario_id" , this->socket_id );
        emit this->dataReceivedSignal( json );
    });
}


