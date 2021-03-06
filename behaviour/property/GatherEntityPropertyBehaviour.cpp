#include "GatherAgentPropertyBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../app/App.h"

QString GatherAgentPropertyBehaviour::NEXTS = "nexts";
QString GatherAgentPropertyBehaviour::GENERATING_AGENT_ID = "emitting_agent_id";
QString GatherAgentPropertyBehaviour::PROPERTY_TO_GATHER = "gather_property";

GatherAgentPropertyBehaviour::GatherAgentPropertyBehaviour() : GWSBehaviour(){

}


QPair< double , QJsonArray > GatherAgentPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QJsonValue emitting_agent_id = this->getProperty( GENERATING_AGENT_ID );
    QJsonValue emitting_agent_property = this->getProperty( PROPERTY_TO_GATHER );

    QSharedPointer< GWSAgent > emitting_agent =  GWSAgentEnvironment::globalInstance()->getByClassAndUID( GWSAgent::staticMetaObject.className() , emitting_agent_id.toString() );

    QString property_to_gather = emitting_agent_property.toString();

    double new_waste = emitting_agent->getProperty( property_to_gather ).toDouble() + agent->getProperty( property_to_gather ).toDouble();

    emitting_agent->setProperty( property_to_gather  , 0. );
    agent->setProperty( property_to_gather , new_waste );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
