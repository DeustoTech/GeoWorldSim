#include "FindClosestBehaviour.h"
#include "../../util/routing/DijkstraRouting.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "ContainerAgent.h"


QString FindClosestBehaviour::CLOSEST_AGENT_TYPE = "closest_agent_type";  // e.g. glass ContainerAgent
QString FindClosestBehaviour::TRANSPORT_NETWORK_TYPE = "transport_network_type";  // e.g. glass ContainerAgent
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


   /* QVariant reference_x = this->getProperty( CLOSEST_FROM_X );
    QVariant reference_y = this->getProperty( CLOSEST_FROM_Y );

    // Check if the reference (X, Y) position was inputted as agent's property:
    bool reference_x_is_property = reference_x.toString().startsWith( "<" ) && reference_x.toString().endsWith( ">" );
    bool reference_y_is_property = reference_y.toString().startsWith( "<" ) && reference_y.toString().endsWith( ">" );

    if (reference_x_is_property && reference_y_is_property ){

        QString reference_x_property_name = reference_x.toString().remove( 0 , 1 );
        reference_x_property_name = reference_x_property_name.remove(reference_x_property_name.length() - 1 , 1 );
        reference_x = agent->getProperty( reference_x_property_name );

        QString reference_y_property_name = reference_y.toString().remove( 0 , 1 );
        reference_y_property_name = reference_y_property_name.remove(reference_y_property_name.length() - 1 , 1 );
        reference_y = agent->getProperty( reference_y_property_name );

    }

    GWSCoordinate reference_coord = GWSCoordinate( reference_x.toDouble(), reference_y.toDouble() );*/

    GWSCoordinate agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();


    // Set agent type to search:
    QMap<QString , GWSCoordinate > agents_to_search_id_coord_array;
    QList < GWSCoordinate > agents_to_search_coord_array;

    QList<QSharedPointer<GWSAgent> > agents_to_search = GWSAgentEnvironment::globalInstance()->getByClass( CLOSEST_AGENT_TYPE) ;
    foreach ( QSharedPointer<GWSAgent> a, agents_to_search  ){

         GWSCoordinate agents_to_search_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( a )->getCentroid();
         QString agents_to_search_id = a->getProperty("@id").toString();
         agents_to_search_coord_array.append( agents_to_search_coord ) ;
         agents_to_search_id_coord_array.insert( agents_to_search_id , agents_to_search_coord );

     }

    // Generate graph if non-existing:
    if( !this->routing_graph ){
        this->generateGraph();
    }

    // Obtain closest agent coordinates:
    this->closest_coor = this->routing_graph->dijkstraNearestNode( agent_coor , agents_to_search_coord_array);

    // Extract and store its ID:
    QString closest_id = agents_to_search_id_coord_array.key( this->closest_coor );
    agent->setProperty( STORE_CLOSEST_ID_AS , closest_id );

    // Extract and store the route to it:
    QList< QSharedPointer<GWSGraphEdge> > closest_route = this->routing_graph->dijkstraShortestPath( agent_coor , this->closest_coor );
    //agent->setProperty(STORE_CLOSEST_ROUTE_AS , closest_route );

    // Extract and store the distance of the route:
    GWSLengthUnit closest_route_distance = 0;
    foreach ( QSharedPointer<GWSGraphEdge> edge , closest_route ){
        closest_route_distance = closest_route_distance + edge->getLength();
    }
    agent->setProperty( STORE_CLOSEST_ROUTE_DISTANCE_AS , closest_route_distance );

    // Set next behaviours:
    QStringList next = this->getProperty( NEXTS ).toStringList();
    return next;
}
