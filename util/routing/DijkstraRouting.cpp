#include "DijkstraRouting.h"

#include <QDebug>

#include <lemon/bellman_ford.h>
#include <lemon/path.h>

GWSDijkstraRouting::GWSDijkstraRouting( QList<QSharedPointer<GWSGraphEdge> > edges ) : GWSRouting( edges ){
}

GWSDijkstraRouting::~GWSDijkstraRouting(){
}

/**********************************************************************
 METHODS
**********************************************************************/

/**
 * @brief DijkstraRoutingGraph::dijkstraShortestPath Given the ordered nodes to go through, calculates the entire route
 * @param edges
 * @param go_through_nodes
 * @return
 */
QList<QList<QSharedPointer<GWSGraphEdge> > > GWSDijkstraRouting::dijkstraShortestPath(QList< GWSCoordinate > ordered_coors ){
    QList<QList<QSharedPointer<GWSGraphEdge> > > result_routes;

    Dijkstra<ListDigraph, GWSGraphEdgeArcMap >* dijkstra_algorithm = 0;
    GWSGraphEdgeArcMap* routing_graph_costs = 0;

    for(int i = 0; i < ordered_coors.size()-1; i++){

        QList<QSharedPointer<GWSGraphEdge> > result_route;
        GWSCoordinate from_coor = ordered_coors.at( i );
        GWSCoordinate to_coor = ordered_coors.at( i+1 );

        if( !from_coor.isValid() || !to_coor.isValid() ){
            qDebug() << QString("Start (%1) or end coordinates (%2) are not valid").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( result_route );
            continue;
        }

        if( from_coor == to_coor ){
            qDebug() << QString("Same start and end coordinates (%1) (%2)").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( result_route );
            continue;
        }

        // Move to nearest edges
        GWSCoordinate moved_from_coor = this->findNearestRoutingCoordinate( from_coor );
        GWSCoordinate moved_to_coor = this->findNearestRoutingCoordinate( to_coor );

        // Compute dijkstra shortest path
        ListDigraph::Node start = this->coors_to_node.value( moved_from_coor );
        ListDigraph::Node end = this->coors_to_node.value( moved_to_coor );

        if ( this->routing_graph->id( start ) < 0 || this->routing_graph->id( end ) < 0 ){
            qDebug() << QString("Start (%1) or end coordinate (%2) are not in graph").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( result_route );
            continue;
        }

        if( !dijkstra_algorithm ){
            routing_graph_costs = new GWSGraphEdgeArcMap( this );
            dijkstra_algorithm = new Dijkstra<ListDigraph, GWSGraphEdgeArcMap>( *this->routing_graph , *routing_graph_costs );
        }

        if( !dijkstra_algorithm->run( start , end ) ){
            qWarning() << QString("Can not reach end coordinate (%2) from start (%1)").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( result_route );
            continue;
        }

        // Get route
        Path<ListDigraph> shortest_path = dijkstra_algorithm->path( end );
        for(int i = 0 ; i < shortest_path.length() ; i++) {
            ListDigraph::Arc arc = shortest_path.nth( i );
            result_route.append( this->arc_to_edges.value( arc ) );
        }
        result_routes.append( result_route );
    }

    if( dijkstra_algorithm ){
        delete dijkstra_algorithm;
        delete routing_graph_costs;
    }
    return result_routes;
}

/**
 * @brief DijkstraRoutingGraph::dijkstraShortestPath Given a star and a list of end nodes calculates all the routes
 * @param edges
 * @param start_node
 * @param end_nodes
 * @return
 */
QList<QList< QSharedPointer<GWSGraphEdge> > > GWSDijkstraRouting::dijkstraShortestPaths(GWSCoordinate from_coor, QList< GWSCoordinate > to_coors ){
    QList<QList<QSharedPointer<GWSGraphEdge> > > result_routes;

    GWSGraphEdgeArcMap* routing_graph_costs = new GWSGraphEdgeArcMap( this );
    Dijkstra<ListDigraph, GWSGraphEdgeArcMap >* dijkstra_algorithm = new Dijkstra<ListDigraph, GWSGraphEdgeArcMap>( *this->routing_graph , *routing_graph_costs );

    // Get start node and start graph from it
    ListDigraph::Node start = this->coors_to_node.value( from_coor );
    dijkstra_algorithm->run( start );

    // Iterate all end nodes
    foreach( GWSCoordinate to_coor , to_coors ){
        QList< QSharedPointer<GWSGraphEdge>> route;

        ListDigraph::Node end = this->coors_to_node.value( to_coor );

        if( !dijkstra_algorithm->run( start , end ) ){
            qWarning() << QString("Can not reach end node (%2) from start (%1)").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( route );
            continue;
        }

        // Get route
        Path<ListDigraph> shortest_path = dijkstra_algorithm->path( end );
        for(int i = 0 ; i < shortest_path.length() ; i++) {
            ListDigraph::Arc arc = shortest_path.nth( i );
            route.append( this->arc_to_edges.value( arc ) );
        }

        result_routes.append( route );
    }

    delete dijkstra_algorithm;
    delete routing_graph_costs;

    return result_routes;
}

/**
 * @brief DijkstraRoutingGraph::dijkstraShortestPath Given a start and end node, calculates the route
 * @param edges
 * @param start_node
 * @param end_node
 * @return
 */
QList< QSharedPointer<GWSGraphEdge> > GWSDijkstraRouting::dijkstraShortestPath(GWSCoordinate from_coor, GWSCoordinate to_coor ){

    QList< GWSCoordinate > coors;
    coors.append( from_coor );
    coors.append( to_coor );
    return this->dijkstraShortestPath(coors).at(0);
}

/**
 * @brief DijkstraRoutingGraph::dijkstraNearestNode Given a set of GraphNodes, calculates the routing nearest one
 * @param edges Edges to create the graph
 * @param nodes
 * @return
 */
/*QSharedPointer<GWSGraphNode> GWSDijkstraRouting::dijkstraNearestNode(QSharedPointer<GWSGraphNode> from_node, QList< QSharedPointer<GWSGraphNode> > to_nodes ){

    QSharedPointer<GWSGraphNode> result_node = 0;
    GWSLengthUnit min_length( std::numeric_limits<double>::max() );

    QList<QList<QSharedPointer<GWSGraphEdge> > > routes = this->dijkstraShortestPaths(from_node , to_nodes);

    if( routes.size() != to_nodes.size() ){
        qWarning() << "Node list and routes size do not match";
    }

    for(int node_pos = 0; node_pos < routes.size() && node_pos < to_nodes.size() ; node_pos++){
        GWSLengthUnit c;

        foreach( QSharedPointer<GWSGraphEdge> edge , routes.at(node_pos) ){
            c = c + edge->getLength();
        }

        if( c <= min_length ){
            min_length = c;
            result_node = to_nodes.at( node_pos );
        }
    }

    return result_node;
}*/
