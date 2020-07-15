#include "Routing.h"

geoworldsim::routing::Routing::Routing() : QObject(){
    this->arc_to_object_ids = new QMap< lemon::ListDigraph::Arc , std::string >();
    this->hash_to_node = new QMap< std::string , lemon::ListDigraph::Node >();
    this->routing_graph = new lemon::ListDigraph();
    this->graph_edge_visitor = new graph::EdgeVisitor( this->routing_graph );

    this->connect( this , &geoworldsim::routing::Routing::upsertEdgeSignal , this , &geoworldsim::routing::Routing::upsertEdge );
    this->connect( this , &geoworldsim::routing::Routing::removeEdgeSignal , this , &geoworldsim::routing::Routing::removeEdge );

    qRegisterMetaType< graph::Edge >( "graph::Edge" );
}

geoworldsim::routing::Routing::~Routing(){
    delete this->arc_to_object_ids;
    delete this->hash_to_node;
    delete this->routing_graph;
    delete this->graph_edge_visitor;
}

/**********************************************************************
 GETTERS
**********************************************************************/

QStringList geoworldsim::routing::Routing::getShortestPath( const QString &from_hash , const QString &to_hash ){
    return this->getShortestPath( { from_hash , to_hash } ).at(0);
}


QList< QStringList > geoworldsim::routing::Routing::getShortestPath( const QStringList &ordered_hashes ){
    QList< QStringList > result_routes;

    lemon::Dijkstra< lemon::ListDigraph, graph::EdgeVisitor > dijkstra_algorithm = lemon::Dijkstra< lemon::ListDigraph , graph::EdgeVisitor >( *this->routing_graph , *this->graph_edge_visitor );

    for(int i = 0; i < ordered_hashes.size()-1; i++){

        QStringList route;
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
        lemon::ListDigraph::Node start = this->hash_to_node->value( from_hash.toStdString() );
        lemon::ListDigraph::Node end = this->hash_to_node->value( to_hash.toStdString() );

        if( start == end ){
            result_routes.append( route );
            continue;
        }

        // Check in cache
        if( this->routes_cache.keys().contains( start ) && this->routes_cache.value( start ).keys().contains( end ) ){
            result_routes.append( this->routes_cache.value( start ).value( end ) );
            continue;
        }

        if ( this->routing_graph->id( start ) <= 0 || this->routing_graph->id( end ) <= 0 ){
            qDebug() << QString("Start (%1) or end nodes (%2) are not in graph").arg( from_hash ).arg( to_hash );
            this->cachePath( start , end , route );
            result_routes.append( route ); // route = empty QStringList since either/both nodes are not in the Graph
            continue;
        }

        if( !dijkstra_algorithm.run( start , end ) ){
            qDebug() << QString("Can not reach end node (%2) from start (%1)").arg( from_hash ).arg( to_hash );
            this->cachePath( start , end , route );
            result_routes.append( route ); // route = empty QStringList since the dijkstra_algorithm could not find a path
            continue;
        }

        // Get route
        lemon::Path<lemon::ListDigraph> shortest_path = dijkstra_algorithm.path( end );
        for(int i = 0 ; i < shortest_path.length() ; i++) {
            lemon::ListDigraph::Arc arc = shortest_path.nth( i );
            route.append( QString::fromStdString( this->arc_to_object_ids->value( arc ) ) );
        }

        // Save in cache
        this->cachePath( start , end , route );

        result_routes.append( route );
    }
    return result_routes;
}


QList< QStringList > geoworldsim::routing::Routing::getShortestPaths( const QString &from_one_hash , const QStringList &to_many_hashes ){
    QList< QStringList > result_routes;

    if( from_one_hash.isEmpty() ){
        qWarning() << QString("Start (%1) not defined").arg( from_one_hash );
        for(int i = 0; i < to_many_hashes.size() ; i++){
            result_routes.append( QStringList() );
        }
        return result_routes;
    }

    // Compute dijkstra shortest path
    lemon::ListDigraph::Node start = this->hash_to_node->value( from_one_hash.toStdString()  );

    if ( this->routing_graph->id( start ) < 0 ){
        qWarning() << QString("Start (%1) is not in graph").arg( from_one_hash );
        return result_routes;
    }

    // Get start node and start graph from it
    lemon::Dijkstra< lemon::ListDigraph, graph::EdgeVisitor > dijkstra_algorithm = lemon::Dijkstra< lemon::ListDigraph , graph::EdgeVisitor >( *this->routing_graph , *this->graph_edge_visitor );

    dijkstra_algorithm.run( start );

    // Iterate all end nodes
    foreach( QString to_hash , to_many_hashes ){
        QStringList route = QStringList();

        if( to_hash.isEmpty() ){
            result_routes.append( route );
            continue;
        }

        lemon::ListDigraph::Node end = this->hash_to_node->value( to_hash.toStdString() );

        // Check in cache
        this->mutex.lockForRead();
        if( this->routes_cache.keys().contains( start ) && this->routes_cache.value( start ).keys().contains( end ) ){
            result_routes.append( this->routes_cache.value( start ).value( end ) );
            this->mutex.unlock();
            continue;
        }
        this->mutex.unlock();

        if ( this->routing_graph->id( end ) < 0 ){
            this->cachePath( start , end , route );
            result_routes.append( route );
            continue;
        }

        // Get route
        lemon::Path<lemon::ListDigraph> shortest_path = dijkstra_algorithm.path( end );

        for(int i = 0 ; i < shortest_path.length() ; i++) {
            lemon::ListDigraph::Arc arc = shortest_path.nth( i );
            this->mutex.lockForRead();
            if( this->arc_to_object_ids->keys().contains( arc ) ){
                route.append( QString::fromStdString( this->arc_to_object_ids->value( arc ) ) );
            }
            this->mutex.unlock();
        }

        // Save in cache
        this->cachePath( start , end , route );

        result_routes.append( route );
    }

    return result_routes;
}


/**********************************************************************
 METHODS
**********************************************************************/

void geoworldsim::routing::Routing::upsertEdge(const QString &object_id , const graph::Edge &edge){

    try {

        // Create or retrieve edge start node
        QString from_hash = edge.getFromNodeUID();
        lemon::ListDigraph::Node s = this->hash_to_node->value( from_hash.toStdString() ); // Start node

        this->mutex.lockForWrite();
        if( this->routing_graph->id( s ) <= 0 ){
            s = this->routing_graph->addNode();
            this->hash_to_node->insert( from_hash.toStdString() , s );
        }
        this->mutex.unlock();

        // Create or retrieve edge end node
        QString to_hash = edge.getToNodeUID();
        lemon::ListDigraph::Node e = this->hash_to_node->value( to_hash.toStdString() );

        this->mutex.lockForWrite();
        if( this->routing_graph->id( e ) <= 0 ){
            e = this->routing_graph->addNode();
            this->hash_to_node->insert( to_hash.toStdString() , e );
        }

        // Create arc and link it to edge

        lemon::ListDigraph::Arc arc = this->routing_graph->addArc( s , e );
        this->arc_to_object_ids->insert( arc , object_id.toStdString() );
        this->graph_edge_visitor->edges.insert( arc , edge );
        this->mutex.unlock();

    } catch(...){}
}

void geoworldsim::routing::Routing::removeEdge(const QString &object_id ){

    lemon::ListDigraph::Arc arc = this->arc_to_object_ids->key( object_id.toStdString() );
    if( this->routing_graph->id( arc ) > 0 ){
        this->routing_graph->erase( arc );
        this->arc_to_object_ids->remove( arc );
    }
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void geoworldsim::routing::Routing::cachePath( lemon::ListDigraph::Node start , lemon::ListDigraph::Node end , QStringList route ){

    this->mutex.lockForRead();
    if( !this->routes_cache.keys().contains( start ) ){
    this->mutex.unlock();

        this->mutex.lockForWrite();
        this->routes_cache.insert( start , QMap< lemon::ListDigraph::Node , QStringList >() );
        this->mutex.unlock();
    } else {
        this->mutex.unlock();
    }

    this->mutex.lockForWrite();
    this->routes_cache[ start ].insert( end , route );
    this->mutex.unlock();
}
