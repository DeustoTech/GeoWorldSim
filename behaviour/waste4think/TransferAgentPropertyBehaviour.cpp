#include "TransferAgentPropertyBehaviour.h"

#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString TransferAgentPropertyBehaviour::PROPERTY_TO_TRANSFER = "property_to_transfer";
QString TransferAgentPropertyBehaviour::RECEIVING_AGENT_ID = "receiving_agent_id";
QString TransferAgentPropertyBehaviour::NEXTS = "nexts";


TransferAgentPropertyBehaviour::TransferAgentPropertyBehaviour() : GWSBehaviour()
{

}

QStringList TransferAgentPropertyBehaviour::behave(){

    QSharedPointer< GWSAgent > agent = this->getAgent();
    QVariant closest_agent_id = this->getProperty( RECEIVING_AGENT_ID );

    bool is_property = closest_agent_id.toString().startsWith( "<" ) && closest_agent_id.toString().endsWith( ">" );

    if (is_property ){
        QString property_name = closest_agent_id.toString().remove( 0 , 1);
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        closest_agent_id = agent->getProperty( property_name );
    }

    QSharedPointer< GWSAgent > closest_agent =  GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , closest_agent_id.toString() );

    double new_waste = closest_agent->getProperty( this->getProperty( PROPERTY_TO_TRANSFER ).toString() ).toDouble() + agent->getProperty( this->getProperty( PROPERTY_TO_TRANSFER ).toString() ).toDouble();
    closest_agent->setProperty( this->getProperty( PROPERTY_TO_TRANSFER ).toString() , new_waste );
    agent->setProperty( this->getProperty( PROPERTY_TO_TRANSFER ).toString() , 0.);

    emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent->serialize() );
    emit GWSApp::globalInstance()->sendAgentToSocketSignal( closest_agent->serialize() );

    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;

}
