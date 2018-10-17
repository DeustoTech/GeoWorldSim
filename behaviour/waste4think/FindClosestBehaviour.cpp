#include "FindClosestBehaviour.h"
#include "../../util/routing/DijkstraRouting.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "ContainerAgent.h"


QString FindClosestBehaviour::CLOSEST_AGENT_TYPE = "closest_agent_type";  // e.g. ContainerAgent
QString FindClosestBehaviour::CLOSEST_AGENT_SUBTYPE = "closest_agent_subtype"; // e.g. glass
QString FindClosestBehaviour::CLOSEST_ID = "closest_id";
QString FindClosestBehaviour::NEXTS = "nexts";


FindClosestBehaviour::FindClosestBehaviour() : GWSBehaviour()
{

}


void FindClosestBehaviour::generateGraph(){

    // Generate graph of all GWSAgents in the network environment
    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( "GWSAgent" );

    // Get all the Edges from the graph
    this->routing_graph = new GWSDijkstraRouting( graph->getEdges() );
}

QStringList FindClosestBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_home_coord = GWSCoordinate( agent->getProperty( "home_coordX" ).toDouble(), agent->getProperty( "home_coordY" ).toDouble() );

    QMap<QString , GWSCoordinate > agents_to_search_id_coord_array;
    QList < GWSCoordinate > agents_to_search_coord_array;

    // CAREFUL!! WE NEED TO SPECIFY THE WASTE TYPE WE WANT TO THROW!
    QList<QSharedPointer<GWSAgent> > agents_to_search = GWSAgentEnvironment::globalInstance()->getByClass( CLOSEST_AGENT_TYPE) ;
    foreach ( QSharedPointer<GWSAgent> a, agents_to_search  ){

         GWSCoordinate agents_to_search_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( a )->getCentroid();
         QString agents_to_search_id = a->getProperty("@id").toString();
         agents_to_search_coord_array.append( agents_to_search_coord ) ;
         agents_to_search_id_coord_array.insert( agents_to_search_id , agents_to_search_coord );

     }

    if( !this->routing_graph ){
        this->generateGraph();
    }

    this->closest_coor = this->routing_graph->dijkstraNearestNode( agent_home_coord , agents_to_search_coord_array);
    QString closest_id = agents_to_search_id_coord_array.key( this->closest_coor );
    agent->setProperty( CLOSEST_ID , closest_id );

    QStringList next = this->getProperty( NEXTS );
    return next;
}
