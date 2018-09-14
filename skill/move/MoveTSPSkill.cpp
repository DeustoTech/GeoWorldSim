#include "MoveTSPSkill.h"
#include "../../environment/Environment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString MoveTSPSkill::EDGES_CLASS_PROP = "edges_class";
QString MoveTSPSkill::ROUTE_DESTINATION_X_PROP = "route_destination_x";
QString MoveTSPSkill::ROUTE_DESTINATION_Y_PROP = "route_destination_y";


MoveTSPSkill::MoveTSPSkill() : MoveSkill(){
    this->setProperty( EDGES_CLASS_PROP , "Road" );
}

MoveTSPSkill::~MoveTSPSkill(){
    if( this->routing_graph ){
        this->routing_graph->deleteLater();
    }
}


/**********************************************************************
 GETTERS
**********************************************************************/

// Generate graph, give me all the agents in the network environment, and generate the graph with them

void MoveTSPSkill::generateGraph(){
    // Generate graph of all Road Edges in the network environment
    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( this->getProperty( MoveTSPSkill::EDGES_CLASS_PROP ).toString() );

    // Generate Dijkstra Routing algorithm from graph edges
    this->routing_graph = new GWSDijkstraRouting( graph->getEdges() );
}

GWSCoordinate MoveTSPSkill::getRouteDestination() const{
    if( this->getProperty( ROUTE_DESTINATION_X_PROP ).isNull() || this->getProperty( ROUTE_DESTINATION_Y_PROP ).isNull() ){
        return GWSCoordinate( Q_INFINITY , Q_INFINITY , Q_INFINITY );
    }
    return GWSCoordinate( this->getProperty( ROUTE_DESTINATION_X_PROP ) .toDouble( ) , this->getProperty( ROUTE_DESTINATION_Y_PROP ).toDouble( ) , 0 );
}


/**********************************************************************
 METHODS
**********************************************************************/

void MoveTSPSkill::move( GWSTimeUnit movement_duration ){

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

    // Extract agent coordinates
    GWSCoordinate current_coor = agent_geom->getCentroid();

    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( this->getProperty( MoveTSPSkill::EDGES_CLASS_PROP ).toString() );
    QList< QSharedPointer<GWSGraphEdge> > edges = graph->getEdges();

    foreach ( QSharedPointer<GWSGraphEdge> e, edges ){
        QList< GWSCoordinate > destination_coors = GWSCoordinate( e->getTo().getX(), e->getTo().getY() );
        destination_coors = destination_coors.append(destination_coors);
        //qDebug() << edge_coors;
    }

    if( this->route.isEmpty() ){
        // Generate pending route, i.e. cost matrix
        this->route = this->routing_graph->dijkstraShortestPaths( current_coor , destination_coors);
    }


    // And now that we have the cost matrix, we can call the TSP algorithms
    /*
    GSSTSPRouting::nearestNeighborTsp(GWSGraphNode *start_node, QList<GWSGraphNode *> destination_coors, GWSGraphNode *end_node)


    */





    }





    /*GWSCoordinate agent_coor = agent_geom->getCentroid();
    QList< GWSCoordinate > neighbour_coors = this->routing_graph->getFrom()

    // neighbour_coors are the coordinates of all the edges in the graph
    this->route = this->routing_graph->dijkstraShortestPaths( agent_coor , neighbour_coors);

    // We need to fill the cost matrix up through the GWSDijskstraShortestPaths method
    if( this->route.isEmpty() ){
        // Generate pending route
        this->route = this->routing_graph->dijkstraShortestPaths( agent_coor , neighbour_coors);
    }

    this->routing_graph->*/
}
