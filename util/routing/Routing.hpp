#ifndef GWSROUTING_H
#define GWSROUTING_H

#include <QMutex>
#include <QDebug>
#include <QSharedPointer>
#include "../../object/Object.h"

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

#include "../../util/graph/Edge.h"
#include "../../util/routing/EdgeVisitor.hpp"

template <class T = GWSEdge>
class GWSRouting : public GWSObject
{

public:

    GWSRouting(){
        this->arc_to_edges = new QMap< lemon::ListDigraph::Arc , T >();
        this->hash_to_node = new QMap< QString , lemon::ListDigraph::Node >();
        this->routing_graph = new lemon::ListDigraph();
        this->graph_edge_visitor = new GWSEdgeVisitor<T>( this->routing_graph );

    }

    ~GWSRouting(){
        delete this->arc_to_edges;
        delete this->hash_to_node;
        delete this->routing_graph;
        delete this->graph_edge_visitor;
    }

    /**********************************************************************
     GETTERS - ONE TO ONE
    **********************************************************************/

    QList< T > getShortestPath( QString from_hash , QString to_hash ){
        return this->getShortestPath( { from_hash , to_hash } ).at(0);
    }

    /**********************************************************************
     GETTERS -  ORDERED MANY (ONE AFTER THE OTHER)
    **********************************************************************/

    QList<QList< T > > getShortestPath( QStringList ordered_hashes ){
        QList<QList< T > > result_routes;

        lemon::Dijkstra< lemon::ListDigraph, GWSEdgeVisitor<T> > dijkstra_algorithm = lemon::Dijkstra< lemon::ListDigraph , GWSEdgeVisitor<T> >( *this->routing_graph , *this->graph_edge_visitor );

        for(int i = 0; i < ordered_hashes.size()-1; i++){

            QList< T > route;
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

            // Check in cache
            if( this->routes_cache.keys().contains( start ) && this->routes_cache.value( start ).keys().contains( end ) ){
                result_routes.append( this->routes_cache.value( start ).value( end ) );
                continue;
            }

            if ( this->routing_graph->id( start ) <= 0 || this->routing_graph->id( end ) <= 0 ){
                qDebug() << QString("Start (%1) or end nodes (%2) are not in graph").arg( from_hash ).arg( to_hash );
                this->cachePath( start , end , route );
                result_routes.append( route );
                continue;
            }

            if( !dijkstra_algorithm.run( start , end ) ){
                qWarning() << QString("Can not reach end node (%2) from start (%1)").arg( from_hash ).arg( to_hash );
                this->cachePath( start , end , route );
                this->mutex.unlock();
                result_routes.append( route );
                continue;
            }

            // Get route
            lemon::Path<lemon::ListDigraph> shortest_path = dijkstra_algorithm.path( end );
            for(int i = 0 ; i < shortest_path.length() ; i++) {
                lemon::ListDigraph::Arc arc = shortest_path.nth( i );
                route.append( this->arc_to_edges->value( arc ) );
            }

            // Save in cache
            this->cachePath( start , end , route );

            result_routes.append( route );
        }
        return result_routes;
    }

    /**********************************************************************
     GETTERS -  ONE TO MANY
    **********************************************************************/

    QList<QList< T > > getShortestPaths( QString from_one_hash , QStringList to_many_hashes ){
        QList< QList< T > > result_routes;

        if( from_one_hash.isEmpty() ){
            return result_routes;
        }

        // Compute dijkstra shortest path
        lemon::ListDigraph::Node start = this->hash_to_node->value( from_one_hash  );

        if ( this->routing_graph->id( start ) < 0 ){
            qWarning() << QString("Start (%1) is not in graph").arg( from_one_hash );
            return result_routes;
        }

        // Get start node and start graph from it
        lemon::Dijkstra< lemon::ListDigraph, GWSEdgeVisitor<T> > dijkstra_algorithm = lemon::Dijkstra< lemon::ListDigraph , GWSEdgeVisitor<T> >( *this->routing_graph , *this->graph_edge_visitor );

        dijkstra_algorithm.run( start );

        // Iterate all end nodes
        foreach( QString to_hash , to_many_hashes ){
            QList< T > route = QList< T >();

            if( to_hash.isEmpty() ){
                result_routes.append( route );
                continue;
            }

            lemon::ListDigraph::Node end = this->hash_to_node->value( to_hash );

            // Check in cache
            if( this->routes_cache.keys().contains( start ) && this->routes_cache.value( start ).keys().contains( end ) ){
                result_routes.append( this->routes_cache.value( start ).value( end ) );
                continue;
            }

            if ( this->routing_graph->id( end ) < 0 ){
                this->cachePath( start , end , route );
                result_routes.append( route );
                continue;
            }

            // Get route
            lemon::Path<lemon::ListDigraph> shortest_path = dijkstra_algorithm.path( end );
            for(int i = 0 ; i < shortest_path.length() ; i++) {
                lemon::ListDigraph::Arc arc = shortest_path.nth( i );
                if( this->arc_to_edges->keys().contains( arc ) ){
                    route.append( this->arc_to_edges->value( arc ) );
                }
            }

            // Save in cache
            this->cachePath( start , end , route );

            result_routes.append( route );
        }

        return result_routes;
    }

    /**********************************************************************
     SETTERS - ADD EDGE
    **********************************************************************/

    void upsert(const T& edge){

        //QSharedPointer< GWSEdge > edge = e.dynamicCast<GWSEdge>();

        try {

            // Create or retrieve edge start node
            QString from_hash = edge.getFromNodeUID();
            lemon::ListDigraph::Node s = this->hash_to_node->value( from_hash ); // Start node

            this->mutex.lock();
            if( this->routing_graph->id( s ) <= 0 ){
                s = this->routing_graph->addNode();
                this->hash_to_node->insert( from_hash , s );
            }
            this->mutex.unlock();

            // Create or retrieve edge end node
            QString to_hash = edge.getToNodeUID();
            lemon::ListDigraph::Node e = this->hash_to_node->value( to_hash );

            this->mutex.lock();
            if( this->routing_graph->id( e ) <= 0 ){
                e = this->routing_graph->addNode();
                this->hash_to_node->insert( to_hash , e );
            }

            // Create arc and link it to edge

            lemon::ListDigraph::Arc arc = this->routing_graph->addArc(s , e);
            this->arc_to_edges->insert( arc , edge );
            this->graph_edge_visitor->edges.insert( arc , edge );
            this->mutex.unlock();

        } catch(...){}
    }

    /**********************************************************************
     SETTERS - REMOVE EDGE
    **********************************************************************/

    void remove(const T& edge){

        //QSharedPointer< GWSEdge > edge = e.dynamicCast<GWSEdge>();

        lemon::ListDigraph::Arc arc = this->arc_to_edges->key( edge );
        if( this->routing_graph->id( arc ) > 0 ){
            this->routing_graph->erase( arc );
            this->arc_to_edges->remove( arc );
        }
    }

    /**********************************************************************
     SETTERS - CACHE CALCULATED ROUTES
    **********************************************************************/

    void cachePath( lemon::ListDigraph::Node start , lemon::ListDigraph::Node end , QList< T > route ){
        if( !this->routes_cache.keys().contains( start ) ){
            this->routes_cache.insert( start , QMap< lemon::ListDigraph::Node , QList< T > >() );
        }
        this->routes_cache[ start ].insert( end , route );
    }

protected:

    QMutex mutex;

    // Used to create nodes and arcs
    lemon::ListDigraph* routing_graph = Q_NULLPTR;
    GWSEdgeVisitor<T>* graph_edge_visitor = Q_NULLPTR;

    // To link arcs with its original GSSGraphEdge
    QMap< lemon::ListDigraph::Arc , T >* arc_to_edges;

    // To link nodes with its original GSSGraphNode
    QMap< QString , lemon::ListDigraph::Node >* hash_to_node;

    // Routes cache
    QMap< lemon::ListDigraph::Node , QMap< lemon::ListDigraph::Node , QList< T > > > routes_cache; // QMAP< DEPARTURE , QMAP< DESTINATION , ROUTE > >

};

#endif // GWSROUTING_H
