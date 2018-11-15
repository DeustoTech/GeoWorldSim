#include "ExternalListener.h"

#include <QJsonDocument>

#include "../../agent/Agent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../object/ObjectFactory.h"

GWSExternalListener::GWSExternalListener( QString simulation_id ) : QObject(){
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
    });

    // Keep alive
    QObject::connect( &this->websocket , &QWebSocket::pong , [this](quint64 elapsedTime, const QByteArray &payload){
        Q_UNUSED(elapsedTime); Q_UNUSED(payload);
        emit this->websocket.ping();
    });

    // Events
    QObject::connect( &this->websocket , &QWebSocket::textMessageReceived , this , &GWSExternalListener::messageReceived );
    QObject::connect( &this->websocket , &QWebSocket::disconnected , [this](){
        QTimer::singleShot( 10*1000 , this , &GWSExternalListener::reconnectSocket );
    });

    this->reconnectSocket();
}

void GWSExternalListener::reconnectSocket(){
    this->websocket.open( QUrl( "ws://sockets.geoworldsim.com/?scenario_id=" + this->listening_simulation_id ) );
}

void GWSExternalListener::messageReceived(const QString message){

    //qInfo() << "Received message" << message;
    QJsonObject json = QJsonDocument::fromJson( message.toLatin1() ).object();

    // RECEIVED AN AGENT
    if( json.value("signal") == "entity" ){
        json = json.value("body").toObject();

        QString type = json.value( GWSAgent::GWS_TYPE_PROP ).toString();
        QString id = json.value( GWSAgent::GWS_ID_PROP ).toString();
        bool alive = json.value( GWSAgent::ALIVE_PROP ).toBool();

        QSharedPointer<GWSAgent> agent;

        if( !type.isEmpty() && !id.isEmpty() ){
            agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( type , id );
        }

        if( agent && !alive ){
            agent->deleteLater();
            return;
        }

        if( agent && alive ){
            agent->deserialize( json );
            return;
        }

        if( !agent && alive ){
            agent = GWSObjectFactory::globalInstance()->fromJSON( json ).dynamicCast<GWSAgent>();
        }
    }

}
