#include "SendEntitySnapshotBehaviour.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString geoworldsim::behaviour::SendEntitySnapshotBehaviour::ENTITY_TO_SEND_UID = "entity_to_send_id";
QString geoworldsim::behaviour::SendEntitySnapshotBehaviour::SOCKET_ID = "socket_id";
QString geoworldsim::behaviour::SendEntitySnapshotBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::SendEntitySnapshotBehaviour::SendEntitySnapshotBehaviour() : Behaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QPair<double, QJsonArray> geoworldsim::behaviour::SendEntitySnapshotBehaviour::behave(){

    // Agent to fetch UID (by default this behaviours agent)
    QSharedPointer<Entity> entity = this->getEntity();
    QString entity_to_send_id = this->getProperty( ENTITY_TO_SEND_UID ).toString( entity->getUID() );

    // Socket id to send through (by default the main from the Simulation)
    QString socket_id = this->getProperty( SOCKET_ID ).toString( App::globalInstance()->getAppId() );

    // Fetched agent
    if( entity_to_send_id != entity->getUID() ){
        entity = environment::EntityEnvironment::globalInstance()->getByUID( entity_to_send_id );
    }

    // Send the agent
    this->snapshotEntity( entity );

    QJsonObject return_data = this->getProperties( QStringList({ BEHAVIOUR_DURATION , NEXTS }) );
    return QPair< double , QJsonArray >( return_data.value( BEHAVIOUR_DURATION ).toDouble() , return_data.value( NEXTS ).toArray() );
}
