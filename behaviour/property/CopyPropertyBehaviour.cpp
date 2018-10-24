#include "CopyPropertyBehaviour.h"

#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString CopyPropertyBehaviour::AGENT_ID_TO_COPY_FROM = "agent_id_to_copy_from";
QString CopyPropertyBehaviour::PROPERTY_NAME_TO_COPY = "property_name";
QString CopyPropertyBehaviour::NEXTS = "nexts";

CopyPropertyBehaviour::CopyPropertyBehaviour() : GWSBehaviour(){

}

QStringList CopyPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString agent_id_to_copy_from = this->getProperty( AGENT_ID_TO_COPY_FROM ).toString();

    bool id_is_property = agent_id_to_copy_from.startsWith( "<" ) && agent_id_to_copy_from.endsWith( ">" );

    if ( id_is_property ){
        QString find_in_property = agent_id_to_copy_from.remove( 0 , 1 );
        find_in_property = find_in_property.remove( find_in_property.length() - 1 , 1 );
        agent_id_to_copy_from = agent->getProperty( find_in_property ).toString();
    }

    QString property_name_to_copy = this->getProperty( PROPERTY_NAME_TO_COPY ).toString();

    QSharedPointer<GWSAgent> copy_agent = GWSAgentEnvironment::globalInstance()->getById( agent_id_to_copy_from );
    agent->setProperty( property_name_to_copy , copy_agent->getProperty( property_name_to_copy ) );

    emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent->serialize() );

    // Set next behaviours:
    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;
}
