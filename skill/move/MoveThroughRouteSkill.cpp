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
    }
    GWSCoordinate current_coor = agent_geom->getCentroid();

    // Extract destination coordinates
    GWSCoordinate destination_coor = this->getRouteDestination();

    // Get pending route
    QList< QSharedPointer<GWSGraphEdge> > pending_route = this->routing_graph->dijkstraShortestPath( current_coor , destination_coor );

    if( pending_route.isEmpty() ){
        qDebug() << "Route is empty, may have reached destination";
        return;
    }

    QSharedPointer<GWSGraphEdge> current_edge = pending_route.at(0);
    this->setProperty( MoveThroughRouteSkill::DESTINATION_X_PROP , current_edge->getTo().getX() );
    this->setProperty( MoveThroughRouteSkill::DESTINATION_Y_PROP , current_edge->getTo().getY() );
    MoveSkill::move( movement_duration );

    /*qDebug() << "Step = " <<position.toString();
    this->setProperty( ACCUMULATED_DISTANCE_PROP , this->getAccDistance() + meters );
    this->setProperty( ACCUMULATED_TIME_PROP , this->getAccTime() + movement_duration );*/

}
