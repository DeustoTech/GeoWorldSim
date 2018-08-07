#include "DeleteBehaviour.h"

#include "../../app/App.h"
#include "../../environment/Environment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

DeleteBehaviour::DeleteBehaviour(GWSAgent *behaving_agent) : GWSBehaviour( behaving_agent ){
}

bool DeleteBehaviour::finished(){
    return deleted;
}

bool DeleteBehaviour::behave(){

    this->getAgent()->icon_url = "https://image.flaticon.com/icons/svg/250/250162.svg";
    QTimer::singleShot( this->getProperty( GWSBehaviour::INCREMENT_AGENT_TIME_PROP ).toDouble() , this->getAgent() , &GWSAgent::deleteLater ); // Die in the specified seconds

    GWSExecutionEnvironment::globalInstance()->unregisterAgent( this->getAgent() );
    return this->deleted = true;
}
