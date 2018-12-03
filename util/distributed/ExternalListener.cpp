#include "ExternalListener.h"

#include <QJsonDocument>

#include "../../agent/Agent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../object/ObjectFactory.h"

GWSExternalListener::GWSExternalListener( QString socket_id ) : GWSExternalCommunicator( socket_id ){
    // Listening to yourself would be a loop
    Q_ASSERT( socket_id != GWSApp::globalInstance()->getAppId() );

    this->websocket.connect( &this->websocket , &QWebSocket::connected , [this](){
        this->websocket.connect( &this->websocket , &QWebSocket::textMessageReceived , this , &GWSExternalListener::messageReceived );
    } );
}


void GWSExternalListener::messageReceived(const QString message){

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
