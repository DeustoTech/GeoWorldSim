#include "DeleteAgentBehaviour.h"

#include "../../app/App.h"
#include "../../environment/Environment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

DeleteAgentBehaviour::DeleteAgentBehaviour() : GWSBehaviour( ){
}

bool DeleteAgentBehaviour::canContinueToNext(){
    return deleted;
}

bool DeleteAgentBehaviour::behave(){

    this->getAgent()->icon_url = "https://image.flaticon.com/icons/svg/250/250162.svg";
    emit GWSApp::globalInstance()->pushAgentSignal( this->getAgent()->serialize() );
    GWSExecutionEnvironment::globalInstance()->unregisterAgent( this->getAgent() );

    return this->deleted = true;
}
