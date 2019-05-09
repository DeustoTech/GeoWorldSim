#include "SetNextRouteDestinationBehaviour.h"

#include "../../skill/move/StoreMultiRouteSkill.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"


QString SetNextRouteDestinationBehaviour::NEXTS_WHILE_PENDING_ROUTE = "nexts_while_pending_route";
QString SetNextRouteDestinationBehaviour::NEXTS_IF_ROUTE_FINISHED = "nexts_if_finished";

SetNextRouteDestinationBehaviour::SetNextRouteDestinationBehaviour() : GWSBehaviour() {
}


QPair< double , QJsonArray > SetNextRouteDestinationBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QJsonArray ordered_destinations = agent->getProperty( GWSStoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS ).toArray();

    // Finished following destinations
    if ( ordered_destinations.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_ROUTE_FINISHED ).toArray() );
    }

    // Have some pending destination
    QJsonValue next_destination = ordered_destinations.at( 0 );
    ordered_destinations.removeAt( 0 );
    agent->setProperty( GWSStoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS , ordered_destinations );

    // We have the next agent_id
    if( next_destination.type() == QJsonValue::String ){

        QString next_destination_agent_id = next_destination.toString();
        agent->setProperty( this->getProperty( GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_AGENT_ID_AS ).toString( GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_AGENT_ID_AS ) , next_destination_agent_id );

        QSharedPointer<GWSAgent> destination_agent = GWSAgentEnvironment::globalInstance()->getByUID( next_destination_agent_id );
        GWSCoordinate next_destination_agent_coor = GWSGeometry( destination_agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() ).getCentroid();

        agent->setProperty( this->getProperty( GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_X_VALUE ).toString( GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_X_VALUE ) , next_destination_agent_coor.getX() );
        agent->setProperty( this->getProperty( GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_Y_VALUE ).toString( GWSStoreMultiRouteSkill::STORE_NEXT_DESTINATION_Y_VALUE ) , next_destination_agent_coor.getY() );

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

