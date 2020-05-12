#include "CopyPropertyBehaviour.h"

#include "../../app/App.h"
#include "../../environment/entity_environment/EntityEnvironment.h"

QString geoworldsim::behaviour::CopyPropertyBehaviour::AGENT_ID_TO_COPY_FROM = "agent_id_to_copy_from";
QString geoworldsim::behaviour::CopyPropertyBehaviour::PROPERTY_NAME_TO_COPY = "property_name";
QString geoworldsim::behaviour::CopyPropertyBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::CopyPropertyBehaviour::CopyPropertyBehaviour() : Behaviour(){

}

QPair< double , QJsonArray > geoworldsim::behaviour::CopyPropertyBehaviour::behave(){

    QSharedPointer<Entity> agent = this->getEntity();

    QString agent_id_to_copy_from = this->getProperty( AGENT_ID_TO_COPY_FROM ).toString();

    bool id_is_property = agent_id_to_copy_from.startsWith( "<" ) && agent_id_to_copy_from.endsWith( ">" );

    if ( id_is_property ){
        QString find_in_property = agent_id_to_copy_from.remove( 0 , 1 );
        find_in_property = find_in_property.remove( find_in_property.length() - 1 , 1 );
        agent_id_to_copy_from = agent->getProperty( find_in_property ).toString();
    }

    QString property_name_to_copy = this->getProperty( PROPERTY_NAME_TO_COPY ).toString();

    QSharedPointer<Entity> copy_agent = environment::EntityEnvironment::globalInstance()->getByUID( agent_id_to_copy_from );
    agent->setProperty( property_name_to_copy , copy_agent->getProperty( property_name_to_copy ) );

    // Set next behaviours:
   return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
