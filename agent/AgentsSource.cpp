#include "AgentsSource.h"

#include "../object/ObjectFactory.h"
#include "../environment/agent_environment/AgentEnvironment.h"
#include "../environment/execution_environment/ExecutionEnvironment.h"

QString GWSAgentsSource::CREATION_TYPE_PROP = "creation_type";
QString GWSAgentsSource::CREATION_RATIO_PROP = "creation_ratio";
QString GWSAgentsSource::CREATION_LIMIT_PROP = "creation_limit";
QString GWSAgentsSource::CREATED_AMOUNT_PROP = "created_amount";

GWSAgentsSource::GWSAgentsSource(){

}

void GWSAgentsSource::behave(){

    QString type = this->getProperty( CREATION_TYPE_PROP ).toString();
    if( type.isEmpty() ){
        return;
    }

    // Calculate how many per second
    double amount = 0;

    for(int i = 0 ; i < amount && i < this->getProperty( CREATION_LIMIT_PROP ).toInt() ; i++){
        GWSAgent* agent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromType( type ) );

        //agent->setProperty( GWSAgent::GEOMETRY_PROP )

        GWSAgentEnvironment::globalInstance()->registerAgent( agent );
        GWSExecutionEnvironment::globalInstance()->registerAgent( agent );
    }

    this->setProperty( CREATED_AMOUNT_PROP , this->getProperty( CREATED_AMOUNT_PROP ).toInt() + amount );
    this->incrementInternalTime( 1 );

}
