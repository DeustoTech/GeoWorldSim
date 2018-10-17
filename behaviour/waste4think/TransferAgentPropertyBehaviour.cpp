#include "TransferAgentPropertyBehaviour.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString TransferAgentPropertyBehaviour::PROPERTY_TO_TRANSFER = "transfer_property";
QString TransferAgentPropertyBehaviour::RECEIVING_AGENT_ID = "receiving_agent_id";
QString TransferAgentPropertyBehaviour::ID_FROM_PROPERTY = "id_from_property";
QString TransferAgentPropertyBehaviour::NEXTS = "nexts";


TransferAgentPropertyBehaviour::TransferAgentPropertyBehaviour() : GWSBehaviour()
{

}

QStringList TransferAgentPropertyBehaviour::behave(){

    QString property_to_transfer = this->getProperty( PROPERTY_TO_TRANSFER );

    QSharedPointer< GWSAgent > agent = this->getAgent();
    QString closest_agent_id;

    if ( !this->getProperty( RECEIVING_AGENT_ID ).isNull() ){
       closest_agent_id = this->getProperty( RECEIVING_AGENT_ID ).toString();
    }
    if ( !this->getProperty( ID_FROM_PROPERTY ).isNull() ){
       closest_agent_id = this->getProperty( ID_FROM_PROPERTY ).toString();
    }

    QSharedPointer< GWSAgent > closest_agent =  GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , closest_agent_id );

    double new_waste = closest_agent->getProperty( property_to_transfer ).toDouble() + agent->getProperty( property_to_transfer ).toDouble();
    closest_agent->setProperty( property_to_transfer , new_waste );
    agent->setProperty( property_to_transfer , 0.);
    QStringList nexts = this->getProperty( NEXTS );

    return nexts;

}
