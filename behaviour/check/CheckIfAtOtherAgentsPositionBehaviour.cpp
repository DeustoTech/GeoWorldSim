#include "CheckIfAtOtherAgentsPositionBehaviour.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString CheckIfAtOtherAgentsPositionBehaviour::OTHER_AGENT_ID_PROP = "compare_agent_id";

CheckIfAtOtherAgentsPositionBehaviour::CheckIfAtOtherAgentsPositionBehaviour() : GWSBehaviour() {
}

bool CheckIfAtOtherAgentsPositionBehaviour::continueToNext(){

    QString agent_id = this->getAgent()->getProperty( OTHER_AGENT_ID_PROP ).toString();
    QSharedPointer<GWSAgent> other = GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , agent_id );

    if( !other ){
        return false;
    }

    GWSPhysicalEnvironment * env = GWSPhysicalEnvironment::globalInstance();
    return env->getGeometry( this->getAgent() )->getCentroid() == env->getGeometry( other )->getCentroid();
}

bool CheckIfAtOtherAgentsPositionBehaviour::behave(){
    // EMPTY, DO NOTHING
    return true;
}
