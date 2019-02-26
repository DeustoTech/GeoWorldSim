#include "DeleteAgentBehaviour.h"

#include "../../app/App.h"
#include "../../environment/Environment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"


DeleteAgentBehaviour::DeleteAgentBehaviour() : GWSBehaviour( ){
}


QJsonArray DeleteAgentBehaviour::behave(){

   // emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( json );
    GWSEnvironmentsGroup::globalInstance()->unregisterAgent( this->getAgent() );

    return QJsonArray();
}
