#include "FindClosestBehaviour.h"
#include "../../util/routing/DijkstraRouting.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "ContainerAgent.h"


QString FindClosestBehaviour::CLOSEST_AGENT_TYPE = "closest_agent_type";  // e.g. glass ContainerAgent
QString FindClosestBehaviour::TRANSPORT_NETWORK_TYPE = "transport_network_type";  //
QString FindClosestBehaviour::STORE_CLOSEST_ID_AS = "store_closest_id_as";
//QString FindClosestBehaviour::STORE_CLOSEST_ROUTE_AS = "store_closest_route_as";
QString FindClosestBehaviour::STORE_CLOSEST_ROUTE_DISTANCE_AS = "store_closest_route_distance_as";
//QString FindClosestBehaviour::CLOSEST_FROM_X = "closest_from_x";
//QString FindClosestBehaviour::CLOSEST_FROM_Y = "closest_from_y";
QString FindClosestBehaviour::NEXTS = "nexts";


FindClosestBehaviour::FindClosestBehaviour() : GWSBehaviour(){

}

QStringList FindClosestBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    if ( agent->getProperty( this->getProperty( STORE_CLOSEST_ID_AS ).toString() ).isNull() ){

        // Use future to allo multithreading
        GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();
        QSharedPointer<GWSGeometry> agent_geom = env->getGeometry( agent );
        GWSCoordinate agent_coor = agent_geom->getCentroid();

        // Set agent type to search
        QList< QSharedPointer<GWSAgent> > all_agents_of_type = GWSAgentEnvironment::globalInstance()->getByClass( this->getProperty( CLOSEST_AGENT_TYPE ).toString() ) ;
        QMap< GWSCoordinate , QSharedPointer<GWSAgent> > coor_to_agent;

        foreach ( QSharedPointer<GWSAgent> a, all_agents_of_type  ){
             GWSCoordinate c = env->getGeometry( a )->getCentroid();
             coor_to_agent.insert( c , a );
        }

        // Obtain routes to all agent coordinates
        QList< GWSCoordinate > coors_of_all_agents_of_type = coor_to_agent.keys();
        QPair< GWSCoordinate , QList< QSharedPointer<GWSGraphEdge> > > closest_coor_and_route = GWSNetworkEnvironment::globalInstance()->getNearestNodeAndPath( agent_coor , coors_of_all_agents_of_type , this->getProperty( TRANSPORT_NETWORK_TYPE ).toString() );

        // Extract and store its ID:
        QSharedPointer<GWSAgent> closest_agent = coor_to_agent.value( closest_coor_and_route.first );

        QString save_closest_id_as = this->getProperty( STORE_CLOSEST_ID_AS ).toString();
        if( save_closest_id_as.isEmpty() ){ save_closest_id_as = "closest_agent_id"; }
        agent->setProperty( save_closest_id_as , closest_agent->getId() );


        // Extract and store the route to it:
        QList< QSharedPointer<GWSGraphEdge> > closest_route = closest_coor_and_route.second;

        // Extract and store the distance of the route:
        GWSLengthUnit closest_route_distance = 0;
        foreach ( QSharedPointer<GWSGraphEdge> edge , closest_route ){
            closest_route_distance = closest_route_distance + edge->getLength();
        }
        agent->setProperty( this->getProperty( STORE_CLOSEST_ROUTE_DISTANCE_AS ).toString() , closest_route_distance );


    }

    // Set next behaviours:
    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;
}
