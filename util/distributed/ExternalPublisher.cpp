#include "ExternalPublisher.h"

#include "../../app/App.h"
#include "../../object/Object.h"

#include <QJsonDocument>

GWSExternalPublisher::GWSExternalPublisher(QString socket_id) : GWSExternalCommunicator( socket_id){

}

void GWSExternalPublisher::sendMessage(QString signal , QJsonValue json){

    // TO sockets.geoworldsim.com
    QJsonObject socket_json;
    socket_json.insert( "signal" , signal );
    socket_json.insert( "scenario_id" , GWSApp::globalInstance()->getAppId() );
    socket_json.insert( "body" , json );
    this->websocket.sendTextMessage( QJsonDocument( socket_json ).toJson() );
}
