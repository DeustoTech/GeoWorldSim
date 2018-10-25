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


void FindClosestBehaviour::generateGraph(){

    // Generate graph of all GWSAgents in the network environment
    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( "GWSAgent" );

    // Get all the Edges from the graph
    this->routing_graph = new GWSDijkstraRouting( graph->getEdges() );
}

QStringList FindClosestBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    if ( agent->getProperty( this->getProperty( STORE_CLOSEST_ID_AS ).toString() ).isNull() ){

        // Use future to allo multithreading
        GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();
        QSharedPointer<GWSGeometry> agent_geom = env->getGeometry( agent );
        GWSCoordinate agent_coor = agent_geom->getCentroid();

        // Set agent type to search:
        QMap<QString , GWSCoordinate > agents_to_search_id_coord_array;
        QList < GWSCoordinate > agents_to_search_coord_array;

        QList<QSharedPointer<GWSAgent> > agents_to_search = GWSAgentEnvironment::globalInstance()->getByClass( this->getProperty( CLOSEST_AGENT_TYPE ).toString() ) ;
        foreach ( QSharedPointer<GWSAgent> a, agents_to_search  ){

             GWSCoordinate agents_to_search_coord = env->getGeometry( a )->getCentroid();
             QString agents_to_search_id = a->getProperty("@id").toString();
             agents_to_search_coord_array.append( agents_to_search_coord ) ;
             agents_to_search_id_coord_array.insert( agents_to_search_id , agents_to_search_coord );

         }

        // Generate graph if non-existing:
        if( !this->routing_graph ){
            this->generateGraph();
        }

        // Obtain closest agent coordinates:
        this->closest_coor = this->routing_graph->dijkstraNearestNode( agent_coor , agents_to_search_coord_array );

        // Extract and store its ID:
        QString closest_id = agents_to_search_id_coord_array.key( this->closest_coor );
        QString save_closest_id_as = this->getProperty( STORE_CLOSEST_ID_AS ).toString();
        if( save_closest_id_as.isEmpty() ){ save_closest_id_as = "closest_agent_id"; }
        agent->setProperty( save_closest_id_as , closest_id );


        // Extract and store the route to it:
        QList< QSharedPointer<GWSGraphEdge> > closest_route = this->routing_graph->dijkstraShortestPath( agent_coor , this->closest_coor );
        //agent->setProperty(STORE_CLOSEST_ROUTE_AS , closest_route );

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
