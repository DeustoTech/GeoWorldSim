#include "PastureAgent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"


PastureAgent::PastureAgent() : GWSAgent(){
}


void PastureAgent::behave(){

    double energy = this->getProperty("energy").toDouble();
    this->opacity = energy / 50;

    GWSAgent::behave();
}

