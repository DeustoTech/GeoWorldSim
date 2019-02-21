#include "StopAgentBehaviour.h"

#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

StopAgentBehaviour::StopAgentBehaviour() : GWSBehaviour(){
}



/**********************************************************************
 SLOTS
**********************************************************************/

QJsonArray  StopAgentBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSExecutionEnvironment* env = GWSExecutionEnvironment::globalInstance();
    env->unregisterAgent( agent );

    QJsonObject json = agent->serialize();
    json.insert( GWSPhysicalEnvironment::GEOMETRY_PROP , false );
    json.insert( GWSExecutionEnvironment::AGENT_BIRTH_PROP , false );

    emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( json );

    return QJsonArray();
}
