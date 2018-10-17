#include "StopAgentBehaviour.h"

#include "../../environment/execution_environment/ExecutionEnvironment.h"

StopAgentBehaviour::StopAgentBehaviour() : GWSBehaviour(){
}



/**********************************************************************
 SLOTS
**********************************************************************/

QStringList StopAgentBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSExecutionEnvironment* env = GWSExecutionEnvironment::globalInstance();

    env->unregisterAgent( agent );
}