#include "SendAgentSnapshotBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString SendAgentSnapshotBehaviour::AGENT_TO_SEND_ID = "agent_to_send_id";
QString SendAgentSnapshotBehaviour::NEXTS = "nexts";

SendAgentSnapshotBehaviour::SendAgentSnapshotBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QStringList SendAgentSnapshotBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString agent_to_send_id = this->getProperty( AGENT_TO_SEND_ID ).toString();
    QSharedPointer<GWSAgent> agent_to_send = GWSAgentEnvironment::globalInstance()->getById( agent_to_send_id );
    emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent_to_send->serialize() );

    return this->getNexts( NEXTS );

}
