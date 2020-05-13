#include "SetNextRouteDestinationBehaviour.h"

#include "../../skill/move/StoreMultiRouteSkill.h"
#include "../../behaviour/move/MoveThroughNetworkBehaviour.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"


QString geoworldsim::behaviour::SetNextRouteDestinationBehaviour::NEXTS_WHILE_PENDING_ROUTE = "nexts_while_pending_route";
QString geoworldsim::behaviour::SetNextRouteDestinationBehaviour::NEXTS_IF_ROUTE_FINISHED = "nexts_if_finished";

geoworldsim::behaviour::SetNextRouteDestinationBehaviour::SetNextRouteDestinationBehaviour() : Behaviour() {
}


QPair<double, QJsonArray> geoworldsim::behaviour::SetNextRouteDestinationBehaviour::behave(){

    QSharedPointer<Entity> agent = this->getEntity();
    QJsonArray ordered_destinations = agent->getProperty( geoworldsim::skill::StoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS ).toArray();

    // Finished following destinations
    if ( ordered_destinations.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_ROUTE_FINISHED ).toArray() );
    }

    // Have some pending destination
    QJsonValue next_destination = ordered_destinations.at( 0 );
    ordered_destinations.removeAt( 0 );
    agent->setProperty( geoworldsim::skill::StoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS , ordered_destinations );

    // We have the next agent_id
    if( next_destination.type() == QJsonValue::String ){

        QString next_destination_agent_id = next_destination.toString();

        QSharedPointer<Entity> destination_agent = environment::EntityEnvironment::globalInstance()->getByUID( next_destination_agent_id );
        geometry::Geometry next_destination_agent_geom= geometry::Geometry( destination_agent->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );

        agent->setProperties( QJsonObject({
            { this->getProperty( geoworldsim::skill::StoreMultiRouteSkill::STORE_NEXT_DESTINATION_AGENT_ID_AS ).toString(  geoworldsim::skill::StoreMultiRouteSkill::STORE_NEXT_DESTINATION_AGENT_ID_AS ) , next_destination_agent_id },
            { this->getProperty( geoworldsim::skill::StoreMultiRouteSkill::STORE_NEXT_DESTINATION ).toString(  geoworldsim::skill::StoreMultiRouteSkill::STORE_NEXT_DESTINATION ) , next_destination_agent_geom.getGeoJSON() }
        }) );

        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_WHILE_PENDING_ROUTE ).toArray() );

    }

    // Is an object with several properties, set all of them to agent
    if( next_destination.type() == QJsonValue::Object ){

        foreach( QString key , next_destination.toObject().keys() ) {
            agent->setProperty( key , next_destination.toObject().value( key ) );
        }

        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_WHILE_PENDING_ROUTE ).toArray() );
    }

    // Else

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_ROUTE_FINISHED ).toArray() );
}

