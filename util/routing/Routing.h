#ifndef GWSROUTING_H
#define GWSROUTING_H

#include <QMutex>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

#include "../../util/geometry/Coordinate.h"
#include "../../util/graph/Edge.h"
#include "../../util/routing/EdgeVisitor.h"

class GWSRouting : public GWSObject
{
    Q_OBJECT
    friend class GWSEdgeVisitor;

public:
    GWSRouting();
    ~GWSRouting();

    // GETTERS
    // ONE TO ONE
    template <class T> QList< QSharedPointer< T > > getShortestPath( QString from_hash , QString to_hash ){
        QStringList hashes;
        hashes.append( from_hash );
        hashes.append( to_hash );
        return this->getShortestPath<T>( hashes ).at(0);
    }

    // ORDERED MANY (ONE AFTER THE OTHER)
    QList<QList< QSharedPointer< GWSEdge > > > getShortestPath( QStringList ordered_hashes );
    template <class T> QList< QList< QSharedPointer< T > > > getShortestPath( QStringList ordered_hashes ){
        QList< QList< QSharedPointer< T > > > routes = QList< QList< QSharedPointer< T > > >();
        QList< QList< QSharedPointer< GWSEdge > > > eroutes = this->getShortestPath( ordered_hashes );
        foreach (QList< QSharedPointer< GWSEdge > > eroute , eroutes) {
            QList< QSharedPointer< T > > route = QList< QSharedPointer< T > >();
            foreach (QSharedPointer< GWSEdge > edge , eroute) {
                route.append( edge.dynamicCast<T>() );
            }
            routes.append( route );
        }
        return routes;
    }

    // ONE TO MANY
    QList<QList< QSharedPointer< GWSEdge > > > getShortestPaths( QString from_one_hash , QStringList to_many_hashes );
    template <class T> QList< QList< QSharedPointer< T > > > getShortestPaths( QString from_one_hash , QStringList to_many_hashes ){
        QList< QList< QSharedPointer< T > > > routes = QList< QList< QSharedPointer< T > > >();
        QList< QList< QSharedPointer< GWSEdge > > > eroutes = this->getShortestPaths( from_one_hash , to_many_hashes );
        foreach (QList< QSharedPointer< GWSEdge > > eroute , eroutes) {
            QList< QSharedPointer< T > > route = QList< QSharedPointer< T > >();
            foreach (QSharedPointer< GWSEdge > edge , eroute) {
                route.append( edge.dynamicCast<T>() );
            }
            routes.append( route );
        }
        return routes;
    }

    // SETTERS
    void upsert( QSharedPointer<GWSEdge> edge );
    void remove( QSharedPointer<GWSEdge> edge );

protected:

    QMutex mutex;

    // Used to create nodes and arcs
    lemon::ListDigraph* routing_graph = Q_NULLPTR;
    lemon::Dijkstra< lemon::ListDigraph, GWSEdgeVisitor >* dijkstra_algorithm = Q_NULLPTR;
    GWSEdgeVisitor* graph_edge_visitor = Q_NULLPTR;

    // To link arcs with its original GSSGraphEdge
    QMap< lemon::ListDigraph::Arc , QSharedPointer<GWSEdge> >* arc_to_edges;

    // To link nodes with its original GSSGraphNode
    QMap< QString , lemon::ListDigraph::Node >* hash_to_node;

    // Routes cache
    void cachePath( lemon::ListDigraph::Node start , lemon::ListDigraph::Node end , QList<QSharedPointer<GWSEdge> > route = QList<QSharedPointer<GWSEdge> >() );
    QMap< lemon::ListDigraph::Node , QMap< lemon::ListDigraph::Node , QList<QSharedPointer<GWSEdge> > > > routes_cache; // QMAP< DEPARTURE , QMAP< DESTINATION , ROUTE > >

};

#endif // GWSROUTING_H
