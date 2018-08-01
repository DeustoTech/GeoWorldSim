#include "ExternalListener.h"

#include <QJsonDocument>

#include "../../agent/Agent.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../object/ObjectFactory.h"

GWSExternalListener::GWSExternalListener(QString simulation_id ){
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

    // Remove skills and behaviour
    json.remove( "@skills" );
    json.remove( "@behaviour" );

    QString type = json.value( GWSAgent::GWS_TYPE_PROP ).toString();
    QString id = json.value( GWSAgent::GWS_ID_PROP ).toString();
    QString operation = json.value("operation").toString();

    GWSAgent* agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( type , id );

    if( !agent ){
        GWSAgent* agent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json ) );
    } else {
        agent->deserialize( json );
    }


    /*if( type == GWSEnvironment::globalInstance()->metaObject()->className() ){
        if( operation == "GET" ){ GWSEnvironment::globalInstance()->pushToInterfaceSignal( GWSEnvironment::globalInstance()->toJSON() ); }
        if( operation == "RUN" ){ GWSExecutionEnvironment::globalInstance()->run(); }
        if( operation == "STOP" ){ GWSExecutionEnvironment::globalInstance()->stop(); }
    }
    else
    {
        QString id = json.value("id").toString();
        GWSAgent* agent = Q_NULLPTR;
        QList<GWSAgent*> agents;
        if( id.isEmpty() ){
            agents = GWSEnvironment::globalInstance()->getByClass( type );
        } else {
            agent = GWSEnvironment::globalInstance()->getByClassAndId( type , id );
        }

        if( operation == "RUN" && agent ){ GWSExecutionEnvironment::globalInstance()->runAgent( agent ); }
        if( operation == "RUN" && !agents.isEmpty() ){ GWSExecutionEnvironment::globalInstance()->runAgents( agents ); }
        if( operation == "STOP" && agent ){ GWSExecutionEnvironment::globalInstance()->stopAgent( agent ); }
        if( operation == "STOP" && !agents.isEmpty() ){ GWSExecutionEnvironment::globalInstance()->stopAgents( agents ); }

        if( agent ){ GWSEnvironment::globalInstance()->pushToInterfaceSignal( agent->toJSON() ); }
        if( !agents.isEmpty() ){
            QJsonArray arr;
            foreach( GWSAgent* agent , agents ){ arr.append( agent->toJSON() );}
            GWSEnvironment::globalInstance()->pushToInterfaceSignal( arr );
        }
    }*/

}
