#include "SendAgentSnapshotBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

QString SendAgentSnapshotBehaviour::AGENT_TO_SEND_ID = "agent_to_send_id";
QString SendAgentSnapshotBehaviour::NEXTS = "nexts";

SendAgentSnapshotBehaviour::SendAgentSnapshotBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QJsonArray SendAgentSnapshotBehaviour::behave(){

    QString agent_to_send_id = this->getProperty( AGENT_TO_SEND_ID ).toString();
    QSharedPointer<GWSAgent> agent_to_send = GWSAgentEnvironment::globalInstance()->getById( agent_to_send_id );
    emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( agent_to_send->serialize() );

    return this->getProperty( NEXTS ).toArray();

}
