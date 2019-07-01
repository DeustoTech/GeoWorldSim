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

QPair<double, QJsonArray> SendEntitySnapshotBehaviour::behave(){

    // Agent to fetch UID (by default this behaviours agent)
    QSharedPointer<GWSEntity> entity = this->getEntity();
    QString entity_to_send_id = this->getProperty( ENTITY_TO_SEND_ID ).toString( entity->getUID() );

    // Socket id to send through (by default the main from the Simulation)
    QString socket_id = this->getProperty( SOCKET_ID ).toString( GWSApp::globalInstance()->getAppId() );

    // Fetched agent
    if( entity_to_send_id != entity->getUID() ){
        entity = GWSEntityEnvironment::globalInstance()->getByUID( entity_to_send_id );
    }

    // Send the agent
    if( entity ){
        emit GWSCommunicationEnvironment::globalInstance()->sendEntitySignal( entity->serialize() , socket_id );
    }

    QJsonObject return_data = this->getProperties( QStringList({ BEHAVIOUR_DURATION , NEXTS }) );
    return QPair< double , QJsonArray >( return_data.value( BEHAVIOUR_DURATION ).toDouble() , return_data.value( NEXTS ).toArray() );
}
