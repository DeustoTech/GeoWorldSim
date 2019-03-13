#include "Routing.h"

#include "../../util/routing/EdgeVisitor.h"

/**
 * @brief RoutingGraph::RoutingGraph
 * @param graph
 */
GWSRouting::GWSRouting() : GWSObject(){
    this->arc_to_edges = new QMap< lemon::ListDigraph::Arc , QSharedPointer<GWSEdge> >();
    this->hash_to_node = new QMap< QString , lemon::ListDigraph::Node >();
    this->routing_graph = new lemon::ListDigraph();
    this->graph_edge_visitor = new GWSEdgeVisitor( this->routing_graph );
    this->dijkstra_algorithm = new lemon::Dijkstra< lemon::ListDigraph, GWSEdgeVisitor >( *this->routing_graph , *this->graph_edge_visitor );
}

GWSRouting::~GWSRouting(){
}

/**********************************************************************
 GETTERS
**********************************************************************/


QList< QList< QSharedPointer<GWSEdge> > > GWSRouting::getShortestPath( QStringList ordered_hashes ){
    QList<QList<QSharedPointer<GWSEdge> > > result_routes;

    for(int i = 0; i < ordered_hashes.size()-1; i++){

        QList< QSharedPointer<GWSEdge> > route;
        QString from_hash = ordered_hashes.at( i );
        QString to_hash = ordered_hashes.at( i+1 );

        if( from_hash.isEmpty() || to_hash.isEmpty() ){
            result_routes.append( route );
            continue;
        }

        if( from_hash == to_hash ){
            //qDebug() << QString("Asking for same start and end coordinates routing [%1,%2]").arg( from_coor.getX() ).arg( from_coor.getY() );
            result_routes.append( route );
            continue;
        }

        // Compute dijkstra shortest path
        lemon::ListDigraph::Node start = this->hash_to_node->value( from_hash );
        lemon::ListDigraph::Node end = this->hash_to_node->value( to_hash );

        if( start == end ){
            result_routes.append( route );
            continue;
        }

        this->mutex.lockForRead();

        // Check in cache
        if( this->routes_cache.keys().contains( start ) && this->routes_cache.value( start ).keys().contains( end ) ){
            result_routes.append( this->routes_cache.value( start ).value( end ) );
            this->mutex.unlock();
            continue;
        }

        if ( this->routing_graph->id( start ) < 0 || this->routing_graph->id( end ) < 0 ){
            qDebug() << QString("Start (%1) or end nodes (%2) are not in graph").arg( from_hash ).arg( to_hash );
            this->cachePath( start , end );
            result_routes.append( route );
            this->mutex.unlock();
            continue;
        }

        if( !this->dijkstra_algorithm->run( start , end ) ){
            qWarning() << QString("Can not reach end node (%2) from start (%1)").arg( from_hash ).arg( to_hash );
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

QList<QList<QSharedPointer< GWSEdge > > > GWSRouting::getShortestPaths( QString from_one_hash , QStringList to_many_hashes ){
    QList< QList< QSharedPointer<GWSEdge> > > result_routes;

    // Compute dijkstra shortest path
    lemon::ListDigraph::Node start = this->hash_to_node->value( from_one_hash  );

    this->mutex.lockForRead();
    if ( this->routing_graph->id( start ) < 0 ){
        qWarning() << QString("Start (%1) is not in graph").arg( from_one_hash );
        this->mutex.unlock();
        return result_routes;
    }

    // Get start node and start graph from it
    this->dijkstra_algorithm->run( start );

    // Iterate all end nodes
    foreach( QString to_hash , to_many_hashes ){
        QList< QSharedPointer<GWSEdge> > route = QList< QSharedPointer<GWSEdge> >();

        lemon::ListDigraph::Node end = this->hash_to_node->value( to_hash );

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

void GWSRouting::upsert(QSharedPointer<GWSEdge> edge){

    try {

        // Create or retrieve edge start node
        QString from_hash = edge->getFromNodeId();
        lemon::ListDigraph::Node s = this->hash_to_node->value( from_hash ); // Start node

        this->mutex.lockForWrite();
        if( this->routing_graph->id( s ) <= 0 ){
            s = this->routing_graph->addNode();
            this->hash_to_node->insert( from_hash , s );
        }
        this->mutex.unlock();

        // Create or retrieve edge end node
        QString to_hash = edge->getToNodeId();
        lemon::ListDigraph::Node e = this->hash_to_node->value( to_hash );

        this->mutex.lockForWrite();
        if( this->routing_graph->id( e ) <= 0 ){
            e = this->routing_graph->addNode();
            this->hash_to_node->insert( to_hash , e );
        }
        this->mutex.unlock();

        // Create arc and link it to edge
        this->mutex.lockForWrite();
        lemon::ListDigraph::Arc arc = this->routing_graph->addArc(s , e);
        this->arc_to_edges->insert( arc , edge );
        this->graph_edge_visitor->arc_costs.insert( arc , edge->getEdgeCost() );
        this->mutex.unlock();

    } catch(...){}

}

void GWSRouting::remove(QSharedPointer<GWSEdge> edge){
    lemon::ListDigraph::Arc arc = this->arc_to_edges->key( edge );
    if( this->routing_graph->id( arc ) > 0 ){
        this->routing_graph->erase( arc );
        this->arc_to_edges->remove( arc );
    }
}

void GWSRouting::cachePath( lemon::ListDigraph::Node start , lemon::ListDigraph::Node end , QList<QSharedPointer<GWSEdge> > route ){
    if( !this->routes_cache.keys().contains( start ) ){
        this->routes_cache.insert( start , QMap< lemon::ListDigraph::Node , QList< QSharedPointer< GWSEdge> > >() );
    }
    this->routes_cache[ start ].insert( end , route );
}
