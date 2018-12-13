#include "CommunicationEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../object/ObjectFactory.h"

GWSCommunicationEnvironment* GWSCommunicationEnvironment::globalInstance(){
    static GWSCommunicationEnvironment instance;
    return &instance;
}

GWSCommunicationEnvironment::GWSCommunicationEnvironment() : GWSEnvironment(){
    qInfo() << "CommunicationEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );

    // Create global Simulation Socket
    QString socket_id = GWSApp::globalInstance()->getAppId();
    this->publishers.insert( socket_id , new GWSExternalPublisher( socket_id ) );

    // Connect signals and slots
    this->connect( this , &GWSCommunicationEnvironment::sendAgentSignal , this , &GWSCommunicationEnvironment::sendAgent );
    this->connect( this , &GWSCommunicationEnvironment::sendMessageSignal , this , &GWSCommunicationEnvironment::sendMessage );

}

GWSCommunicationEnvironment::~GWSCommunicationEnvironment(){
}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSCommunicationEnvironment::registerAgent( QSharedPointer<GWSAgent> agent ){
    Q_UNUSED(agent);
}

void GWSCommunicationEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent ){
    Q_UNUSED(agent);
}

void GWSCommunicationEnvironment::connectExternalEnvironment(QString socket_id){

}

void GWSCommunicationEnvironment::disconnectExternalEnvironment(QString socket_id){

}

/**********************************************************************
 LISTENERS
**********************************************************************/

void GWSCommunicationEnvironment::externalEnvironmentReceived(QJsonObject json_data){

    // RECEIVED AN AGENT
    if( json_data.value("signal") == "entity" ){
        json_data = json_data.value("body").toObject();

        QString type = json_data.value( GWSAgent::GWS_TYPE_PROP ).toString();
        QString id = json_data.value( GWSAgent::GWS_ID_PROP ).toString();
        bool alive = json_data.value( GWSAgent::ALIVE_PROP ).toBool();

        QSharedPointer<GWSAgent> agent;

        if( !type.isEmpty() && !id.isEmpty() ){
            agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( type , id );
        }

        if( agent && !alive ){
            agent->deleteLater();
            return;
        }

        if( agent && alive ){
            agent->deserialize( json_data );
            return;
        }

        if( !agent && alive ){
            agent = GWSObjectFactory::globalInstance()->fromJSON( json_data ).dynamicCast<GWSAgent>();
        }

    }
}

/**********************************************************************
 PUBLISHERS
**********************************************************************/

void GWSCommunicationEnvironment::sendAgent(QJsonObject agent_json, QString socket_id){
    this->sendData( "entity" , agent_json , socket_id );
}

void GWSCommunicationEnvironment::sendMessage(QJsonObject message_json, QString socket_id){
    this->sendData( "message" , message_json , socket_id );
}

void GWSCommunicationEnvironment::sendData(QString signal , QJsonObject data , QString socket_id ){
    GWSExternalPublisher* publisher = this->publishers.value( socket_id , 0 );
    if( !publisher ){
        qDebug() << QString("Creating external publisher %1").arg( socket_id );
        publisher = new GWSExternalPublisher( socket_id );
        this->publishers.insert( socket_id , publisher );
    }
    publisher->sendMessage( signal , data );

}
