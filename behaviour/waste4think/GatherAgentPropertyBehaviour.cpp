#include "GatherAgentPropertyBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../app/App.h"

QString GatherAgentPropertyBehaviour::NEXTS = "nexts";
QString GatherAgentPropertyBehaviour::GENERATING_AGENT_ID = "emitting_agent_id";
QString GatherAgentPropertyBehaviour::PROPERTY_TO_GATHER = "gather_property";

GatherAgentPropertyBehaviour::GatherAgentPropertyBehaviour() : GWSBehaviour(){

}


QStringList GatherAgentPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QVariant emitting_agent_id = this->getProperty( GENERATING_AGENT_ID );

    bool is_property = emitting_agent_id.toString().startsWith( "<" ) && emitting_agent_id.toString().endsWith( ">" );

    if (is_property ){
        QString property_name = emitting_agent_id.toString().remove( 0 , 1 );
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        emitting_agent_id = agent->getProperty( property_name );
    }

    QSharedPointer< GWSAgent > emitting_agent =  GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , emitting_agent_id.toString() );

    double new_waste = emitting_agent->getProperty( this->getProperty( PROPERTY_TO_GATHER ).toString() ).toDouble() + agent->getProperty( this->getProperty( PROPERTY_TO_GATHER ).toString() ).toDouble();
    emitting_agent->setProperty( this->getProperty( PROPERTY_TO_GATHER ).toString() , 0. );
    agent->setProperty( this->getProperty( PROPERTY_TO_GATHER ).toString() , new_waste );

    emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent->serialize() );
    emit GWSApp::globalInstance()->sendAgentToSocketSignal( emitting_agent->serialize() );

    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;

}
