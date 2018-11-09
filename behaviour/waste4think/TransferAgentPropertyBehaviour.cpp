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

    QVariant waste_to_transfer = this->getProperty( PROPERTY_TO_TRANSFER );

    bool waste_is_property = waste_to_transfer.toString().startsWith( "<" ) && waste_to_transfer.toString().endsWith( ">" );

    QString waste_name;
    if ( waste_is_property ){
        QString property_name = waste_to_transfer.toString().remove( 0 , 1);
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        waste_to_transfer = agent->getProperty( property_name );
        waste_name = property_name;
        qDebug() << property_name;
        qDebug() << waste_to_transfer;
    }
    qDebug() << agent->serialize();
    qDebug() << waste_to_transfer.toJsonValue();



    QVariant closest_agent_id = this->getProperty( RECEIVING_AGENT_ID );

    bool is_property = closest_agent_id.toString().startsWith( "<" ) && closest_agent_id.toString().endsWith( ">" );

    if ( is_property ){
        QString property_name = closest_agent_id.toString().remove( 0 , 1);
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        closest_agent_id = agent->getProperty( property_name );
    }

    QSharedPointer< GWSAgent > closest_agent =  GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , closest_agent_id.toString() );

    //closest_agent->getProperty( waste_name ) + waste_to_transfer;
    closest_agent->setProperty( waste_name , closest_agent->getProperty( waste_name ) + waste_to_transfer );
    agent->setProperty( waste_name , 0.);

    emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent->serialize() );
    emit GWSApp::globalInstance()->sendAgentToSocketSignal( closest_agent->serialize() );

    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;

}
