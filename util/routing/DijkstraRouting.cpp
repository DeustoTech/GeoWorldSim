#include "DijkstraRouting.h"

#include <QDebug>

#include <lemon/bellman_ford.h>
#include <lemon/path.h>

GWSDijkstraRouting::GWSDijkstraRouting( QList<QSharedPointer<GWSGraphEdge> > edges ) : GWSRouting( edges ){
}

GWSDijkstraRouting::~GWSDijkstraRouting(){
    if( this->dijkstra_algorithm ){
        delete this->dijkstra_algorithm;
        delete this->routing_graph_costs;
    }
}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSDijkstraRouting::generateGraph(){
    if( this->dijkstra_algorithm ){
        delete this->dijkstra_algorithm;
        delete this->routing_graph_costs;
    }
    this->routing_graph_costs = new GWSGraphEdgeArcMap( this );
    this->dijkstra_algorithm = new Dijkstra<ListDigraph, GWSGraphEdgeArcMap>( *this->routing_graph , *routing_graph_costs );
}

/**
 * @brief DijkstraRoutingGraph::dijkstraShortestPath Given the ordered nodes to go through, calculates the entire route
 * @param edges
 * @param go_through_nodes
 * @return
 */
QList<QList<QSharedPointer<GWSGraphEdge> > > GWSDijkstraRouting::dijkstraShortestPath(QList< GWSCoordinate > ordered_coors ){
    QList<QList<QSharedPointer<GWSGraphEdge> > > result_routes;

    for(int i = 0; i < ordered_coors.size()-1; i++){

        QList<QSharedPointer<GWSGraphEdge> > result_route;
        GWSCoordinate from_coor = ordered_coors.at( i );
        GWSCoordinate to_coor = ordered_coors.at( i+1 );

        if( !from_coor.isValid() || !to_coor.isValid() ){
            result_routes.append( result_route );
            continue;
        }

        if( from_coor == to_coor ){
            qDebug() << QString("Asking for same start and end coordinates routing [%1,%2]").arg( from_coor.getX() ).arg( from_coor.getY() );
            result_routes.append( result_route );
            continue;
        }

        // Move to nearest edges
        GWSCoordinate moved_from_coor = this->getNearestRoutingCoordinate( from_coor );
        GWSCoordinate moved_to_coor = this->getNearestRoutingCoordinate( to_coor );

        // Compute dijkstra shortest path
        ListDigraph::Node start = this->coors_to_node.value( moved_from_coor );
        ListDigraph::Node end = this->coors_to_node.value( moved_to_coor );

        if( start == end ){
            result_routes.append( result_route );
            continue;
        }

        if ( this->routing_graph->id( start ) < 0 || this->routing_graph->id( end ) < 0 ){
            qDebug() << QString("Start (%1) or end coordinate (%2) are not in graph").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( result_route );
            continue;
        }

        if( !this->dijkstra_algorithm ){
            this->generateGraph();
        }

        if( !this->dijkstra_algorithm->run( start , end ) ){
            qWarning() << QString("Can not reach end coordinate (%2) from start (%1)").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( result_route );
            continue;
        }

        // Get route
        Path<ListDigraph> shortest_path = this->dijkstra_algorithm->path( end );
        for(int i = 0 ; i < shortest_path.length() ; i++) {
            ListDigraph::Arc arc = shortest_path.nth( i );
            result_route.append( this->arc_to_edges.value( arc ) );
        }
        result_routes.append( result_route );
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
QList<QList< QSharedPointer<GWSGraphEdge> > > GWSDijkstraRouting::dijkstraShortestPaths(GWSCoordinate from_coor, QList< GWSCoordinate > to_many_coors ){
    QList<QList<QSharedPointer<GWSGraphEdge> > > result_routes;

    if( !this->dijkstra_algorithm ){
        this->generateGraph();
    }

    // this this this
    // Move to nearest edges
    GWSCoordinate moved_from_coor = this->getNearestRoutingCoordinate( from_coor );
    //GWSCoordinate moved_to_coor = this->getNearestRoutingCoordinate( to_coor );

    // Compute dijkstra shortest path
    ListDigraph::Node start = this->coors_to_node.value( moved_from_coor );
    //ListDigraph::Node end = this->coors_to_node.value( moved_to_coor );
    // this this this

    // Get start node and start graph from it
    //ListDigraph::Node start = this->coors_to_node.value( from_coor ); this is the original one
    this->dijkstra_algorithm->run( start );

    // Iterate all end nodes
    foreach( GWSCoordinate to_coor , to_many_coors ){
        QList< QSharedPointer<GWSGraphEdge>> route;

        GWSCoordinate moved_to_coor = this->getNearestRoutingCoordinate( to_coor );
        ListDigraph::Node end = this->coors_to_node.value( moved_to_coor );
        //ListDigraph::Node end = this->coors_to_node.value( to_coor ); // this is the original

        if( !this->dijkstra_algorithm->run( start , end ) ){
            qWarning() << QString("Can not reach end node (%2) from start (%1)").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( route );
            continue;
        }

        // Get route
        Path<ListDigraph> shortest_path = this->dijkstra_algorithm->path( end );
        for(int i = 0 ; i < shortest_path.length() ; i++) {
            ListDigraph::Arc arc = shortest_path.nth( i );
            route.append( this->arc_to_edges.value( arc ) );
        }

        result_routes.append( route );
    }

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
GWSCoordinate GWSDijkstraRouting::dijkstraNearestNode(GWSCoordinate from_coor, QList< GWSCoordinate > to_coors ){

    GWSCoordinate result_coor = GWSCoordinate(0, 0); // Caution!!!!
    GWSLengthUnit min_length( std::numeric_limits<double>::max() );

    QList< QList< QSharedPointer<GWSGraphEdge> > > routes = this->dijkstraShortestPaths(from_coor , to_coors);

    if( routes.size() != to_coors.size() ){
        qWarning() << "Node list and routes size do not match";
    }

    for(int coor_pos = 0; coor_pos < routes.size() && coor_pos < to_coors.size() ; coor_pos++){
        GWSLengthUnit c;

        foreach( QSharedPointer<GWSGraphEdge> edge , routes.at(coor_pos) ){
            c = c + edge->getLength();
        }

        if( c <= min_length ){
            min_length = c;
            result_coor = to_coors.at( coor_pos );
        }
    }

    return result_coor;
}
