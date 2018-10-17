#include "TransferAgentPropertyBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString TransferAgentPropertyBehaviour::PROPERTY_TO_TRANSFER = "transfer_property";
QString TransferAgentPropertyBehaviour::RECEIVING_AGENT_ID = "receiving_agent_id";
QString TransferAgentPropertyBehaviour::NEXTS = "nexts";


TransferAgentPropertyBehaviour::TransferAgentPropertyBehaviour() : GWSBehaviour()
{

}

QStringList TransferAgentPropertyBehaviour::behave(){

    QString property_to_transfer = this->getProperty( PROPERTY_TO_TRANSFER );

    QSharedPointer< GWSAgent > agent = this->getAgent();
    QVariant closest_agent_id = this->getProperty( RECEIVING_AGENT_ID );

    bool is_property = closest_agent_id.toString().startsWith( "<" ) && closest_agent_id.toString().endsWith( ">" );

    if (is_property ){

        QString property_name = closest_agent_id.toString().remove( 0 , 1);
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        closest_agent_id = agent->getProperty( property_name );
    }

    closest_agent_id = closest_agent_id.toString();
    QSharedPointer< GWSAgent > closest_agent =  GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , closest_agent_id );

    double new_waste = closest_agent->getProperty( property_to_transfer ).toDouble() + agent->getProperty( property_to_transfer ).toDouble();
    closest_agent->setProperty( property_to_transfer , new_waste );
    agent->setProperty( property_to_transfer , 0.);
    QStringList nexts = this->getProperty( NEXTS );

    return nexts;

}
