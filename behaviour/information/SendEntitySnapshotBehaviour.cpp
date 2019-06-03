#include "SendEntitySnapshotBehaviour.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString SendEntitySnapshotBehaviour::ENTITY_TO_SEND_ID = "entity_to_send_id";
QString SendEntitySnapshotBehaviour::SOCKET_ID = "socket_id";
QString SendEntitySnapshotBehaviour::NEXTS = "nexts";

SendEntitySnapshotBehaviour::SendEntitySnapshotBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QPair< double , QJsonArray > SendEntitySnapshotBehaviour::behave(){

    // Agent to fetch UID (by default this behaviours agent)
    QString entity_to_send_id = this->getProperty( ENTITY_TO_SEND_ID ).toString( this->getEntity()->getUID() );
    QSharedPointer<GWSEntity> entity_to_send = this->getEntity();

    // Socket id to send through (by default the main from the Simulation)
    QString socket_id = this->getProperty( SOCKET_ID ).toString( GWSApp::globalInstance()->getAppId() );

    // Fetched agent
    if( entity_to_send_id != entity_to_send->getUID() ){
        entity_to_send = GWSEntityEnvironment::globalInstance()->getByUID( entity_to_send_id );
    }

    // Send the agent
    if( entity_to_send ){
        emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( entity_to_send->serialize() , socket_id );
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
