#include "StopAgentBehaviour.h"

#include "../../environment/execution_environment/ExecutionEnvironment.h"

StopAgentBehaviour::StopAgentBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

bool StopAgentBehaviour::canContinueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSExecutionEnvironment* env = GWSExecutionEnvironment::globalInstance();

    if( env->containsAgent( agent ) ){
        return false;
    } else {
        return true;
    }

}

/**********************************************************************
 SLOTS
**********************************************************************/

bool StopAgentBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSExecutionEnvironment* env = GWSExecutionEnvironment::globalInstance();

    env->unregisterAgent( agent );
    return true;
}
