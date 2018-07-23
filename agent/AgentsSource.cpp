#include "AgentsSource.h"

#include "../object/ObjectFactory.h"
#include "../environment/agent_environment/AgentEnvironment.h"
#include "../environment/execution_environment/ExecutionEnvironment.h"

GWSAgentsSource::GWSAgentsSource(){

}


void GWSAgentsSource::behave(){

    // Calculate how many per second
    double amount = 0;

    for(int i = 0 ; i < amount ; i++){
        GWSAgent* agent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromType( "GWSAgent" ) );

        //agent->setProperty( GWSAgent::GEOMETRY_PROP )

        GWSAgentEnvironment::globalInstance()->registerAgent( agent );
        GWSExecutionEnvironment::globalInstance()->registerAgent( agent );
    }

    this->incrementInternalTime( 1 );

}
