#include "ExternalListener.h"

#include <QJsonDocument>

#include "../../app/App.h"

GWSExternalListener::GWSExternalListener( QString socket_id ) : GWSExternalCommunicator( socket_id ){

    // Listening to yourself would be a loop
    Q_ASSERT( socket_id != GWSApp::globalInstance()->getAppId() );

    this->websocket.connect( &this->websocket , &QWebSocket::textMessageReceived , [this](const QString message){
        QJsonObject json = QJsonDocument::fromJson( message.toLatin1() ).object();
        emit this->dataReceivedSignal( json );
    });
}
