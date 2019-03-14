#include "SendAgentSnapshotBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString SendAgentSnapshotBehaviour::AGENT_TO_SEND_ID = "agent_to_send_id";
QString SendAgentSnapshotBehaviour::NEXTS = "nexts";

SendAgentSnapshotBehaviour::SendAgentSnapshotBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QJsonArray SendAgentSnapshotBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QString agent_id = agent->getId();

    QString agent_to_send_id = this->getProperty( AGENT_TO_SEND_ID ).toString();
    QSharedPointer<GWSAgent> agent_to_send = GWSAgentEnvironment::globalInstance()->getById( agent_to_send_id );

   // QJsonObject agent_json = agent_to_send->serialize();

    if ( agent_to_send_id != agent_id ){
        agent_to_send->setProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP , GWSTimeEnvironment::globalInstance()->getAgentInternalTime( agent ) ) ;
    }


    emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( agent_to_send->serialize() );

    return this->getProperty( NEXTS ).toArray();
}
