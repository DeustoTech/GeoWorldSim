#include "PastureAgent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"


PastureAgent::PastureAgent(QObject* parent) : GWSAgent( parent ){


}



void PastureAgent::behave(){

    GWSTimeEnvironment::globalInstance()->incrementAgentInternalTime( this , 5);

    /* Send information to website */
    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );


}
