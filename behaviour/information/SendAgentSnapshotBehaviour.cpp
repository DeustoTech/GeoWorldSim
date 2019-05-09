#include "SendAgentSnapshotBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString SendAgentSnapshotBehaviour::AGENT_TO_SEND_ID = "agent_to_send_id";
QString SendAgentSnapshotBehaviour::SOCKET_ID = "socket_id";
QString SendAgentSnapshotBehaviour::NEXTS = "nexts";

SendAgentSnapshotBehaviour::SendAgentSnapshotBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QPair< double , QJsonArray > SendAgentSnapshotBehaviour::behave(){

    // Agent to fetch UID (by default this behaviours agent)
    QString agent_to_send_id = this->getProperty( AGENT_TO_SEND_ID ).toString( this->getAgent()->getUID() );
    QSharedPointer<GWSAgent> agent_to_send = this->getAgent();

    // Socket id to send through (by default the main from the Simulation)
    QString socket_id = this->getProperty( SOCKET_ID ).toString( GWSApp::globalInstance()->getAppId() );

    // Fetched agent
    if( agent_to_send_id != agent_to_send->getUID() ){
        agent_to_send = GWSAgentEnvironment::globalInstance()->getByUID( agent_to_send_id );
    }

    // Send the agent
    if( agent_to_send ){
        emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( agent_to_send->serialize() , socket_id );
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
