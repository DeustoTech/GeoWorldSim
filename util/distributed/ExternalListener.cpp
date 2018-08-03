#include "ExternalListener.h"

#include <QJsonDocument>

#include "../../agent/Agent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../object/ObjectFactory.h"

GWSExternalListener::GWSExternalListener(QString simulation_id ) : QObject(){
    // Listening to yourself would be a loop
    Q_ASSERT( simulation_id != GWSApp::globalInstance()->getAppId() );

    this->listening_simulation_id = simulation_id;
    this->startSocket();
}

void GWSExternalListener::startSocket(){

    qDebug() << "Trying to connect socket";

    // Connect and send info
    QObject::connect( &this->websocket , &QWebSocket::connected , [this](){
        qInfo() << "WebSocket connected successfully to" << this->websocket.peerAddress();
        QJsonObject socket_json;
        socket_json.insert("signal" , "join" );
        socket_json.insert("socket_id" , this->listening_simulation_id );
        this->websocket.sendTextMessage( QJsonDocument( socket_json ).toJson() );
    });

    QObject::connect( &this->websocket , &QWebSocket::pong , [this](quint64 elapsedTime, const QByteArray &payload){
        Q_UNUSED(elapsedTime); Q_UNUSED(payload);
        emit this->websocket.ping();
    });

    QObject::connect( &this->websocket , &QWebSocket::textMessageReceived , this , &GWSExternalListener::messageReceived );
    QObject::connect( &this->websocket , &QWebSocket::disconnected , this , &GWSExternalListener::startSocket );

    this->websocket.open( QUrl( "ws://sockets.deusto.io" ));
}

void GWSExternalListener::messageReceived(const QString message){

    qInfo() << "Received message" << message;
    QJsonObject json = QJsonDocument::fromJson( message.toLatin1() ).object();


    // RECEIVED AN AGENT
    if( json.value("signal") == "entity" ){
        json = json.value("body").toObject();

        QString type = json.value( GWSAgent::GWS_TYPE_PROP ).toString();
        QString id = json.value( GWSAgent::GWS_ID_PROP ).toString();

        GWSAgent* agent = dynamic_cast<GWSAgent*>( GWSAgentEnvironment::globalInstance()->getByClassAndId( type , id ) );
        if( agent ){
            agent->deserialize( json );
            return;
        }

        if( !agent ){
            agent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json ) );
        }

        if( agent ){
            GWSEnvironment::globalInstance()->registerAgent( agent );
        }
    }

}
