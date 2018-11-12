#include "FindClosestBehaviour.h"
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
QString FindClosestBehaviour::STORE_CLOSEST_X_AS = "store_closest_agent_x_as";
QString FindClosestBehaviour::STORE_CLOSEST_Y_AS = "store_closest_agent_y_as";
QString FindClosestBehaviour::NEXTS = "nexts";


FindClosestBehaviour::FindClosestBehaviour() : GWSBehaviour(){

}

QStringList FindClosestBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Use future to allo multithreading
    GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();
    QSharedPointer<GWSGeometry> agent_geom = env->getGeometry( agent );
    GWSCoordinate agent_coor = agent_geom->getCentroid();

    // Set agent type to search
    QList< QSharedPointer<GWSAgent> > all_agents_of_type = GWSAgentEnvironment::globalInstance()->getByClass( this->getProperty( CLOSEST_AGENT_TYPE ).toString() );
    QMap< GWSCoordinate , QString > coor_to_agent;

    foreach ( QSharedPointer<GWSAgent> a, all_agents_of_type  ){
         QSharedPointer<GWSGeometry> geom = env->getGeometry( a );
         if( geom ){
            coor_to_agent.insert( geom->getCentroid() , a->getId() );
         }
    }

    // Obtain routes to all agent coordinates
    QList< GWSCoordinate > coors_of_all_agents_of_type = coor_to_agent.keys();
    QPair< GWSCoordinate , QList< QSharedPointer<GWSGraphEdge> > > closest_coor_and_route = GWSNetworkEnvironment::globalInstance()->getNearestNodeAndPath( agent_coor , coors_of_all_agents_of_type , this->getProperty( TRANSPORT_NETWORK_TYPE ).toString() );

    // Extract and store its ID:
    QString closest_agent_id = coor_to_agent.value( closest_coor_and_route.first );

    QString save_closest_id_as = this->getProperty( STORE_CLOSEST_ID_AS ).toString();
    if( save_closest_id_as.isEmpty() ){ save_closest_id_as = "closest_agent_id"; }
    agent->setProperty( save_closest_id_as , closest_agent_id );

    QString save_closest_x_as = this->getProperty( STORE_CLOSEST_X_AS ).toString();
    if( save_closest_x_as.isEmpty() ){ save_closest_x_as = "closest_agent_x"; }
    agent->setProperty( save_closest_x_as , closest_coor_and_route.first.getX() );

    QString save_closest_y_as = this->getProperty( STORE_CLOSEST_Y_AS ).toString();
    if( save_closest_y_as.isEmpty() ){ save_closest_y_as = "closest_agent_y"; }
    agent->setProperty( save_closest_y_as , closest_coor_and_route.first.getY() );

    // Extract and store the route to it:
    QList< QSharedPointer<GWSGraphEdge> > closest_route = closest_coor_and_route.second;

    // Extract and store the distance of the route:
    GWSLengthUnit closest_route_distance = 0;
    foreach ( QSharedPointer<GWSGraphEdge> edge , closest_route ){
        closest_route_distance = closest_route_distance + edge->getLength();
    }
    agent->setProperty( this->getProperty( STORE_CLOSEST_ROUTE_DISTANCE_AS ).toString() , closest_route_distance );

    // Set next behaviours:
    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;
}
