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

QPair< double , QJsonArray > StopAgentBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSExecutionEnvironment* env = GWSExecutionEnvironment::globalInstance();
    env->unregisterAgent( agent );

    QJsonObject json = agent->serialize();
    emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( json );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , QJsonArray({ this->getUID() }) );
}
