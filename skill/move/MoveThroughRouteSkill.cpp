#include "MoveThroughRouteSkill.h"
#include "../../environment/Environment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString MoveThroughRouteSkill::EDGES_CLASS_PROP = "edges_class";
QString MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP = "route_destination_x";
QString MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP = "route_destination_y";

MoveThroughRouteSkill::MoveThroughRouteSkill() : MoveSkill(){
    this->setProperty( EDGES_CLASS_PROP , "GWSAgent" );
}

MoveThroughRouteSkill::~MoveThroughRouteSkill(){
    if( this->routing_graph ){
        this->routing_graph->deleteLater();
    }
}

/**********************************************************************
 GETTERS
**********************************************************************/

// Generate graph, give me all the agents in the network environment, and generate the graph with them

void MoveThroughRouteSkill::generateGraph(){
    // Generate graph of all GWSAgents in the network environment
    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( this->getProperty( MoveThroughRouteSkill::EDGES_CLASS_PROP ).toString() );

    // Get all the Edges from the graph
    this->routing_graph = new GWSDijkstraRouting( graph->getEdges() );
}



GWSCoordinate MoveThroughRouteSkill::getRouteDestination() const{
    if( this->getProperty( ROUTE_DESTINATION_X_PROP ).isNull() || this->getProperty( ROUTE_DESTINATION_Y_PROP ).isNull() ){
        return GWSCoordinate( Q_INFINITY , Q_INFINITY , Q_INFINITY );
    }
    return GWSCoordinate( this->getProperty( ROUTE_DESTINATION_X_PROP ) .toDouble( ) , this->getProperty( ROUTE_DESTINATION_Y_PROP ).toDouble( ) , 0 );
}

/**********************************************************************
 METHODS
**********************************************************************/

void MoveThroughRouteSkill::move( GWSTimeUnit movement_duration ){

    if( !this->routing_graph ){
        this->generateGraph();
    }

    // Extract current coordinates of Skilled GWSAgent
    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    if( !agent_geom ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getId() );
        return;
    }


    // Extract destination coordinates
    GWSCoordinate current_coor = agent_geom->getCentroid();
    GWSCoordinate destination_coor = this->getRouteDestination();

    if( current_coor == destination_coor ){
        return;
    }

    if( this->pending_route.isEmpty() ){
        // Generate pending route
        this->pending_route = this->routing_graph->dijkstraShortestPath( current_coor , destination_coor);
    }

    if( pending_route.isEmpty() ){ // Assume we have reached route end, free move to destination
        this->setProperty( MoveSkill::DESTINATION_X_PROP , destination_coor.getX() );
        this->setProperty( MoveSkill::DESTINATION_Y_PROP , destination_coor.getY() );
    } else {

        // Move along route edges
        QSharedPointer<GWSGraphEdge> current_edge = pending_route.at(0);

        // Check if we have reached current_edge
        GWSCoordinate current_edge_end = current_edge->getTo();
        this->setProperty( MoveSkill::DESTINATION_X_PROP , current_edge_end.getX() );
        this->setProperty( MoveSkill::DESTINATION_Y_PROP , current_edge_end.getY() );

        if( current_coor == current_edge_end ){
            this->pending_route.removeAt(0);
        }
    }

    MoveSkill::move( movement_duration );


}
