#include "Routing.h"

#include "../../util/routing/GraphEdgeVisitor.h"

/**
 * @brief RoutingGraph::RoutingGraph
 * @param graph
 */
GWSRouting::GWSRouting() : GWSObject(){
    this->routing_graph = new lemon::ListDigraph();
    this->graph_edge_visitor = new GWSGraphEdgeVisitor( this->routing_graph , this );
    this->dijkstra_algorithm = new lemon::Dijkstra< lemon::ListDigraph, GWSGraphEdgeVisitor >( *this->routing_graph , *this->graph_edge_visitor );
}

GWSRouting::~GWSRouting(){
    // Delete created stuff
    this->arc_to_edges.clear();
}

/**********************************************************************
 GETTERS
**********************************************************************/

QList<QSharedPointer<GWSGraphEdge> > GWSRouting::getShortestPath( GWSCoordinate from_coor, GWSCoordinate to_coor ){
    QList< GWSCoordinate > coors;
    coors.append( from_coor );
    coors.append( to_coor );
    return this->getShortestPath(coors).at(0);
}

QList<QList<QSharedPointer<GWSGraphEdge> > > GWSRouting::getShortestPath(QList< GWSCoordinate > ordered_coors ){
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
            //qDebug() << QString("Asking for same start and end coordinates routing [%1,%2]").arg( from_coor.getX() ).arg( from_coor.getY() );
            result_routes.append( result_route );
            continue;
        }

        // Compute dijkstra shortest path
        lemon::ListDigraph::Node start = this->coors_to_node.value( from_coor );
        lemon::ListDigraph::Node end = this->coors_to_node.value( to_coor );

        if( start == end ){
            result_routes.append( result_route );
            continue;
        }

        if ( this->routing_graph->id( start ) < 0 || this->routing_graph->id( end ) < 0 ){
            qDebug() << QString("Start (%1) or end coordinate (%2) are not in graph").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( result_route );
            continue;
        }

        if( !this->dijkstra_algorithm->run( start , end ) ){
            qWarning() << QString("Can not reach end coordinate (%2) from start (%1)").arg( from_coor.toString() ).arg( to_coor.toString() );
            result_routes.append( result_route );
            continue;
        }

        // Get route
        lemon::Path<lemon::ListDigraph> shortest_path = this->dijkstra_algorithm->path( end );
        for(int i = 0 ; i < shortest_path.length() ; i++) {
            lemon::ListDigraph::Arc arc = shortest_path.nth( i );
            result_route.append( this->arc_to_edges.value( arc ) );
        }
        result_routes.append( result_route );
    }

    return result_routes;
}

QList<QList<QSharedPointer< GWSGraphEdge> > > GWSRouting::getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many ){
    QList< QList< QSharedPointer<GWSGraphEdge> > > result_routes;

    // Compute dijkstra shortest path
    lemon::ListDigraph::Node start = this->coors_to_node.value( from_one );

    if ( this->routing_graph->id( start ) < 0 ){
        qDebug() << QString("Start (%1) is not in graph").arg( from_one.toString() );
        return result_routes;
    }

    // Get start node and start graph from it
    this->dijkstra_algorithm->run( start );

    // Iterate all end nodes
    foreach( GWSCoordinate to_coor , to_many ){
        QList< QSharedPointer<GWSGraphEdge> > route;

        lemon::ListDigraph::Node end = this->coors_to_node.value( to_coor );

        if ( this->routing_graph->id( end ) < 0 ){
            result_routes.append( route );
            continue;
        }

        this->mutex.lock();

        // Get route
        lemon::Path<lemon::ListDigraph> shortest_path = this->dijkstra_algorithm->path( end );
        for(int i = 0 ; i < shortest_path.length() ; i++) {
            lemon::ListDigraph::Arc arc = shortest_path.nth( i );
            route.append( this->arc_to_edges.value( arc ) );
        }

        this->mutex.unlock();

        result_routes.append( route );
    }

    return result_routes;
}


/**********************************************************************
 SETTERS
**********************************************************************/

void GWSRouting::upsert(QSharedPointer<GWSGraphEdge> edge){

    try {

        // Create or retrieve edge start node
        GWSCoordinate from_coor = edge->getFrom();
        lemon::ListDigraph::Node s = this->coors_to_node.value( from_coor ); // Start node

        if( this->routing_graph->id( s ) <= 0 ){
            s = this->routing_graph->addNode();
            this->coors_to_node.insert( from_coor , s );
        }

        // Create or retrieve edge end node
        GWSCoordinate to_coor = edge->getTo();
        lemon::ListDigraph::Node e = this->coors_to_node.value( to_coor );

        if( this->routing_graph->id( e ) <= 0 ){
            e = this->routing_graph->addNode();
            this->coors_to_node.insert( to_coor , e );
        }

        // Create arc and link it to edge
        lemon::ListDigraph::Arc arc = this->routing_graph->addArc(s , e);
        this->arc_to_edges.insert( arc , edge );

    } catch(...){}

}

void GWSRouting::remove(QSharedPointer<GWSGraphEdge> edge){

    lemon::ListDigraph::Arc arc = this->arc_to_edges.key( edge );
    if( this->routing_graph->id( arc ) > 0 ){
        this->routing_graph->erase( arc );
        this->arc_to_edges.remove( arc );
    }

}

