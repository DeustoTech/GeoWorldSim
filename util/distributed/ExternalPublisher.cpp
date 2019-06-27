#include "ExternalPublisher.h"

#include "../../app/App.h"
#include "../../object/Object.h"

#include <QTimer>
#include <QJsonDocument>

GWSExternalPublisher::GWSExternalPublisher(const QString &socket_id) : GWSExternalCommunicator( socket_id){

}

void GWSExternalPublisher::sendMessage(const QString &signal , const QJsonValue &json){

    QTimer::singleShot( 10 , [signal , json , this](){

        // TO sockets.geoworldsim.com
        QJsonObject socket_json;
        socket_json.insert( "signal" , signal );
        socket_json.insert( "scenario_id" , this->socket_id );
        socket_json.insert( "body" , json );
        this->websocket.sendTextMessage( QJsonDocument( socket_json ).toJson() );

    });

}
