#include "FindRoutingClosestBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/network_environment/NetworkEdge.h"

#include "../../util/geometry/GeometryGetters.h"

QString geoworldsim::behaviour::FindRoutingClosestBehaviour::ENTITY_TYPE_TO_FIND = "entity_type_to_find";  // e.g. glass ContainerAgent
QString geoworldsim::behaviour::FindRoutingClosestBehaviour::TRANSPORT_NETWORK_TYPE = "transport_network_type";
QString geoworldsim::behaviour::FindRoutingClosestBehaviour::STORE_ROUTING_CLOSEST_ID_AS = "store_routing_closest_id_as";
QString geoworldsim::behaviour::FindRoutingClosestBehaviour::STORE_ROUTING_CLOSEST_ROUTE_AS = "store_routing_closest_route_as";
QString geoworldsim::behaviour::FindRoutingClosestBehaviour::STORE_ROUTING_DISTANCE_AS = "store_routing_distance_as";
QString geoworldsim::behaviour::FindRoutingClosestBehaviour::STORE_ROUTING_CLOSEST_GEOM_AS = "store_routing_closest_geometry_as";
QString geoworldsim::behaviour::FindRoutingClosestBehaviour::NEXTS = "nexts";
QString geoworldsim::behaviour::FindRoutingClosestBehaviour::NEXTS_IF_NO_ROUTING_CLOSEST_FOUND = "nexts_if_no_routing_closest_found";


geoworldsim::behaviour::FindRoutingClosestBehaviour::FindRoutingClosestBehaviour() : Behaviour(){

}

QPair< double , QJsonArray > geoworldsim::behaviour::FindRoutingClosestBehaviour::behave(){

    QSharedPointer<Entity> entity = this->getEntity();
    geometry::Geometry entity_geom = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    geometry::Coordinate entity_coor = entity_geom.getCentroid();

    // Set agent type to search
    QList< QSharedPointer< Entity > > all_agents_of_type = environment::EntityEnvironment::globalInstance()->getByClass( this->getProperty( ENTITY_TYPE_TO_FIND ).toString() );
    QMap< geometry::Coordinate , QSharedPointer< Entity > > coor_to_entity;

    foreach ( QSharedPointer< Entity > a , all_agents_of_type  ){
         geometry::Geometry geom = a->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject();
         if( geom.isValid() ){
            coor_to_entity.insert( geom.getCentroid() , a );
         }
    }

    // Obtain routes to all agent coordinates
    QList< geometry::Coordinate > coors_of_all_entities_of_type = coor_to_entity.keys();
    QPair< geometry::Coordinate , QList< QSharedPointer< Entity > > >  closest_coor_and_route =
            environment::NetworkEnvironment::globalInstance()->getNearestNodeAndPath( entity_coor , coors_of_all_entities_of_type , this->getProperty( TRANSPORT_NETWORK_TYPE ).toString() );

    // Extract and store the route to nearest node it:
    QList< QSharedPointer< Entity > > route = closest_coor_and_route.second;

    // If agent can not be connected to road network nearest node. Closest nearest node is null
    if ( route.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NO_ROUTING_CLOSEST_FOUND ).toArray() );
    }

    // Extract and store the distance of the route:
    unit::LengthUnit closest_route_distance = 0;

    {
        // From Agent to route start
        geometry::Geometry closest_geom = route.at( 0 )->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject();
        geometry::Coordinate iterating_coor = closest_geom.getCentroid();

        closest_route_distance = closest_route_distance + entity_coor.getDistance( iterating_coor );

        // During route start til route end
        foreach ( QSharedPointer< Entity > edge , route ){

            geometry::Geometry edge_geom = edge->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject();
            geometry::Coordinate edge_coor = edge_geom.getCentroid();
            closest_route_distance = closest_route_distance + iterating_coor.getDistance( edge_coor );
            iterating_coor = edge_coor;
        }

        // From route end to nearest_agent
        geometry::Geometry geom = route.at( route.size() - 1 )->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject();
        geometry::Coordinate coor = geom.getCentroid();
        closest_route_distance = closest_route_distance + closest_coor_and_route.first.getDistance( coor );
    }

    // Extract and store closest node ID and coordinates:
    QSharedPointer< Entity > closest_agent = coor_to_entity.value( closest_coor_and_route.first );
    geometry::Geometry closest_agent_geom = closest_agent->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject();

    // Save results back to agent
    entity->setProperties( QJsonObject({
                        { this->getProperty( STORE_ROUTING_CLOSEST_ID_AS ).toString( "routing_closest_entity_uid" ) , closest_agent->getUID() } ,
                        { this->getProperty( STORE_ROUTING_CLOSEST_GEOM_AS ).toString( "routing_closest_entity_geometry" ) , closest_agent_geom.getGeoJSON() } ,
                        { this->getProperty( STORE_ROUTING_DISTANCE_AS ).toString("routing_closest_entity_distance") , closest_route_distance.number() } } ) );

    QJsonObject geojson;
    geojson.insert( "type" , "LineString" );

    QJsonArray coordinates;
    coordinates.append( QJsonArray({ entity_coor.getX() , entity_coor.getY() , entity_coor.getZ() }) );

    foreach ( QSharedPointer< Entity > edge , closest_coor_and_route.second ) {

        geometry::Geometry edge_geom = edge->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject();
        foreach( geometry::Coordinate edge_coor , geometry::GeometryGetters::getCoordinates( edge_geom ) ){
            coordinates.append( QJsonArray( { edge_coor.getX() , edge_coor.getY() , edge_coor.getZ() } ) );
        }
    }
    coordinates.append( QJsonArray({ closest_coor_and_route.first.getX() , closest_coor_and_route.first.getY() , closest_coor_and_route.first.getZ() }) );
    geojson.insert( "coordinates" , coordinates );
    entity->setProperty( this->getProperty( STORE_ROUTING_CLOSEST_ROUTE_AS ).toString("store_routing_closest_route_as") , geojson );

    // Set next behaviours:
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
