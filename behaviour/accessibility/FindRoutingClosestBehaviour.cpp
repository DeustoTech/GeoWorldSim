#include "FindRoutingClosestBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/network_environment/NetworkEdge.h"

QString FindRoutingClosestBehaviour::ENTITY_TO_ACCESS_TYPE = "facility_type";  // e.g. glass ContainerAgent
QString FindRoutingClosestBehaviour::TRANSPORT_NETWORK_TYPE = "transport_network_type";  //
QString FindRoutingClosestBehaviour::STORE_ROUTING_CLOSEST_ID_AS = "store_routing_closest_id_as";
QString FindRoutingClosestBehaviour::STORE_ROUTING_CLOSEST_ROUTE_AS = "store_routing_closest_route_as";
QString FindRoutingClosestBehaviour::STORE_ROUTING_DISTANCE_AS = "store_routing_distance_as";
QString FindRoutingClosestBehaviour::STORE_ROUTING_CLOSEST_X_AS = "store_routing_closest_agent_x_as";
QString FindRoutingClosestBehaviour::STORE_ROUTING_CLOSEST_Y_AS = "store_routing_closest_agent_y_as";
QString FindRoutingClosestBehaviour::NEXTS = "nexts";
QString FindRoutingClosestBehaviour::NEXTS_IF_NO_ROUTING_CLOSEST_FOUND = "nexts_if_no_routing_closest_found";


FindRoutingClosestBehaviour::FindRoutingClosestBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > FindRoutingClosestBehaviour::behave(){

    QSharedPointer<GWSEntity> agent = this->getEntity();

    GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();
    GWSGeometry agent_geom = env->getGeometry( agent );
    GWSCoordinate agent_coor = agent_geom.getCentroid();

    // Set agent type to search
    QList< QSharedPointer<GWSEntity> > all_agents_of_type = GWSEntityEnvironment::globalInstance()->getByClass( this->getProperty( ENTITY_TO_ACCESS_TYPE ).toString() );
    QMap< GWSCoordinate , QString > coor_to_agent;

    foreach ( QSharedPointer<GWSEntity> a, all_agents_of_type  ){
         GWSGeometry geom = env->getGeometry( a );
         if( geom.isValid() ){
            coor_to_agent.insert( geom.getCentroid() , a->getUID() );
         }
    }

    // Obtain routes to all agent coordinates
    QList< GWSCoordinate > coors_of_all_agents_of_type = coor_to_agent.keys();
    QPair< GWSCoordinate , QList< QSharedPointer<GWSEntity> > >  closest_coor_and_route =
            GWSNetworkEnvironment::globalInstance()->getNearestNodeAndPath( agent_coor , coors_of_all_agents_of_type , this->getProperty( TRANSPORT_NETWORK_TYPE ).toString() );

    // Extract and store the route to nearest node it:
    QList< QSharedPointer<GWSEntity> > closest_route = closest_coor_and_route.second;

    // If agent can not be connected to road network nearest node. Closest nearest node is null
    if ( closest_route.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NO_ROUTING_CLOSEST_FOUND ).toArray() );
    }

    // Extract and store the distance of the route:
    GWSLengthUnit closest_route_distance = 0;

    {
        // From Agent to route start
        GWSGeometry closest_geom = closest_route.at( 0 )->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject();
        GWSCoordinate closest_coor = closest_geom.getCentroid();

        closest_route_distance = closest_route_distance + agent_coor.getDistance( closest_coor );

        // During route start til route end
        foreach ( QSharedPointer<GWSEntity> edge , closest_route ){

            GWSGeometry edge_geom = edge->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject();
            GWSCoordinate edge_coor = edge_geom.getCentroid();
            closest_route_distance = closest_route_distance + agent_coor.getDistance( edge_coor );
        }

        // From route end to nearest_agent
        GWSGeometry geom = closest_route.at( closest_route.size() - 1 )->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject();
        GWSCoordinate coor = geom.getCentroid();
        closest_route_distance = closest_route_distance + closest_coor_and_route.first.getDistance( coor );
    }

    // Extract and store closest node ID and coordinates:
    QString closest_agent_id = coor_to_agent.value( closest_coor_and_route.first );

    // Save results back to agent
    agent->setProperty( this->getProperty( STORE_ROUTING_CLOSEST_ID_AS ).toString( "routing_closest_agent_id" ) , closest_agent_id );
    agent->setProperty( this->getProperty( STORE_ROUTING_CLOSEST_X_AS ).toString( "routing_closest_agent_x" ) , closest_coor_and_route.first.getX() );
    agent->setProperty( this->getProperty( STORE_ROUTING_CLOSEST_Y_AS ).toString( "routing_closest_agent_y" ) , closest_coor_and_route.first.getY() );
    agent->setProperty( this->getProperty( STORE_ROUTING_DISTANCE_AS ).toString("routing_closest_agent_distance") , closest_route_distance.number() );

    QJsonObject geojson;
    geojson.insert( "type" , "LineString" );

    QJsonArray coordinates;
    coordinates.append( QJsonArray({ agent_coor.getX() , agent_coor.getY() , agent_coor.getZ() }) );

    foreach ( QSharedPointer<GWSEntity> edge , closest_coor_and_route.second ) {

        GWSGeometry edge_geom = edge->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject();
        GWSCoordinate edge_coor = edge_geom.getCentroid();

        coordinates.append( QJsonArray( { edge_coor.getX() , edge_coor.getY() , edge_coor.getZ() } ) );

    }
    coordinates.append( QJsonArray({ closest_coor_and_route.first.getX() , closest_coor_and_route.first.getY() , closest_coor_and_route.first.getZ() }) );
    geojson.insert( "coordinates" , coordinates );
    agent->setProperty( this->getProperty( STORE_ROUTING_CLOSEST_ROUTE_AS ).toString("store_routing_closest_route_as") , geojson );

    // Set next behaviours:
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
