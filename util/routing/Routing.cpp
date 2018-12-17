#include "Routing.h"

#include "../../util/routing/GraphEdgeVisitor.h"

/**
 * @brief RoutingGraph::RoutingGraph
 * @param graph
 */
GWSRouting::GWSRouting() : GWSObject(){
    this->arc_to_edges = new QMap< lemon::ListDigraph::Arc , QSharedPointer<GWSNetworkEdge> >();
    this->coors_to_node = new QMap< GWSCoordinate , lemon::ListDigraph::Node >();
    this->routing_graph = new lemon::ListDigraph();
    this->graph_edge_visitor = new GWSGraphEdgeVisitor( this->routing_graph );
    this->dijkstra_algorithm = new lemon::Dijkstra< lemon::ListDigraph, GWSGraphEdgeVisitor >( *this->routing_graph , *this->graph_edge_visitor );
}

GWSRouting::~GWSRouting(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

QList<QSharedPointer<GWSNetworkEdge> > GWSRouting::getShortestPath( GWSCoordinate from_coor, GWSCoordinate to_coor ){
    QList< GWSCoordinate > coors;
    coors.append( from_coor );
    coors.append( to_coor );
    return this->getShortestPath(coors).at(0);
}

QList<QList<QSharedPointer<GWSNetworkEdge> > > GWSRouting::getShortestPath(QList< GWSCoordinate > ordered_coors ){
    QList<QList<QSharedPointer<GWSNetworkEdge> > > result_routes;

    for(int i = 0; i < ordered_coors.size()-1; i++){

        QList<QSharedPointer<GWSNetworkEdge> > route;
        GWSCoordinate from_coor = ordered_coors.at( i );
        GWSCoordinate to_coor = ordered_coors.at( i+1 );

        if( !from_coor.isValid() || !to_coor.isValid() ){
            result_routes.append( route );
            continue;
        }

        if( from_coor == to_coor ){
            //qDebug() << QString("Asking for same start and end coordinates routing [%1,%2]").arg( from_coor.getX() ).arg( from_coor.getY() );
            result_routes.append( route );
            continue;
        }

        // Compute dijkstra shortest path
        lemon::ListDigraph::Node start = this->coors_to_node->value( from_coor );
        lemon::ListDigraph::Node end = this->coors_to_node->value( to_coor );

        if( start == end ){
            result_routes.append( route );
            continue;
        }

        this->mutex.lock();

        // Check in cache
        if( this->routes_cache.keys().contains( start ) && this->routes_cache.value( start ).keys().contains( end ) ){
            result_routes.append( this->routes_cache.value( start ).value( end ) );
            this->mutex.unlock();
            continue;
        }

        if ( this->routing_graph->id( start ) < 0 || this->routing_graph->id( end ) < 0 ){
            qDebug() << QString("Start (%1) or end coordinate (%2) are not in graph").arg( from_coor.toString() ).arg( to_coor.toString() );
            this->cachePath( start , end );
            result_routes.append( route );
            this->mutex.unlock();
            continue;
        }

        if( !this->dijkstra_algorithm->run( start , end ) ){
            qWarning() << QString("Can not reach end coordinate (%2) from start (%1)").arg( from_coor.toString() ).arg( to_coor.toString() );
            this->cachePath( start , end );
            result_routes.append( route );
            this->mutex.unlock();
            continue;
        }

        // Get route
        lemon::Path<lemon::ListDigraph> shortest_path = this->dijkstra_algorithm->path( end );
        for(int i = 0 ; i < shortest_path.length() ; i++) {
            lemon::ListDigraph::Arc arc = shortest_path.nth( i );
            route.append( this->arc_to_edges->value( arc ) );
        }

        // Save in cache
        this->cachePath( start , end , route );

        result_routes.append( route );
        this->mutex.unlock();
    }

    return result_routes;
}

QList<QList<QSharedPointer< GWSNetworkEdge> > > GWSRouting::getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many ){
    QList< QList< QSharedPointer<GWSNetworkEdge> > > result_routes;

    // Compute dijkstra shortest path
    lemon::ListDigraph::Node start = this->coors_to_node->value( from_one );

    this->mutex.lock();
    if ( this->routing_graph->id( start ) < 0 ){
        qWarning() << QString("Start (%1) is not in graph").arg( from_one.toString() );
        this->mutex.unlock();
        return result_routes;
    }

    // Get start node and start graph from it
    this->dijkstra_algorithm->run( start );

    // Iterate all end nodes
    foreach( GWSCoordinate to_coor , to_many ){
        QList< QSharedPointer<GWSNetworkEdge> > route = QList< QSharedPointer<GWSNetworkEdge> >();

        lemon::ListDigraph::Node end = this->coors_to_node->value( to_coor );

        // Check in cache
        if( this->routes_cache.keys().contains( start ) && this->routes_cache.value( start ).keys().contains( end ) ){
            result_routes.append( this->routes_cache.value( start ).value( end ) );
            continue;
        }

        if ( this->routing_graph->id( end ) < 0 ){
            this->cachePath( start , end );
            result_routes.append( route );
            continue;
        }

        // Get route
        if( !this->dijkstra_algorithm->run( start , end ) ){
            this->cachePath( start , end );
            result_routes.append( route );
            continue;
        }

        lemon::Path<lemon::ListDigraph> shortest_path = this->dijkstra_algorithm->path( end );
        for(int i = 0 ; i < shortest_path.length() ; i++) {
            lemon::ListDigraph::Arc arc = shortest_path.nth( i );
            route.append( this->arc_to_edges->value( arc ) );
        }

        // Save in cache
        this->cachePath( start , end , route );

        result_routes.append( route );
    }
    this->mutex.unlock();

    return result_routes;
}


/**********************************************************************
 SETTERS
**********************************************************************/

void GWSRouting::upsert(QSharedPointer<GWSNetworkEdge> edge){

    try {

        // Create or retrieve edge start node
        GWSCoordinate from_coor = edge->getFrom();
        lemon::ListDigraph::Node s = this->coors_to_node->value( from_coor ); // Start node

        this->mutex.lock();
        if( this->routing_graph->id( s ) <= 0 ){
            s = this->routing_graph->addNode();
            this->coors_to_node->insert( from_coor , s );
        }
        this->mutex.unlock();

        // Create or retrieve edge end node
        GWSCoordinate to_coor = edge->getTo();
        lemon::ListDigraph::Node e = this->coors_to_node->value( to_coor );

        this->mutex.lock();
        if( this->routing_graph->id( e ) <= 0 ){
            e = this->routing_graph->addNode();
            this->coors_to_node->insert( to_coor , e );
        }
        this->mutex.unlock();

        // Create arc and link it to edge
        this->mutex.lock();
        lemon::ListDigraph::Arc arc = this->routing_graph->addArc(s , e);
        this->arc_to_edges->insert( arc , edge );
        this->graph_edge_visitor->arc_costs.insert( arc , edge->getCost() );
        this->mutex.unlock();

    } catch(...){}

}

void GWSRouting::remove(QSharedPointer<GWSNetworkEdge> edge){
    lemon::ListDigraph::Arc arc = this->arc_to_edges->key( edge );
    if( this->routing_graph->id( arc ) > 0 ){
        this->routing_graph->erase( arc );
        this->arc_to_edges->remove( arc );
    }
}


void GWSRouting::cachePath( lemon::ListDigraph::Node start , lemon::ListDigraph::Node end , QList<QSharedPointer<GWSNetworkEdge> > route ){
    if( !this->routes_cache.keys().contains( start ) ){
        this->routes_cache.insert( start , QMap< lemon::ListDigraph::Node , QList< QSharedPointer< GWSNetworkEdge> > >() );
    }
    this->routes_cache[ start ].insert( end , route );
}
