#ifndef GWSROUTING_H
#define GWSROUTING_H

#include <QObject>
#include <QReadWriteLock>
#include <QMap>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

#include "../../util/graph/Edge.h"
#include "../../util/routing/EdgeVisitor.hpp"

class GWSRouting : public QObject
{
    Q_OBJECT

public:

    GWSRouting();
    ~GWSRouting();

    // GETTERS
    QStringList getShortestPath( QString from_hash , QString to_hash );
    QList< QStringList > getShortestPath( QStringList ordered_hashes );
    QList< QStringList > getShortestPaths( QString from_one_hash , QStringList to_many_hashes );

    // METHODS
    void upsert(const QString &object_id , const GWSEdge &edge );
    void remove( const QString &object_id );

protected:

    void cachePath( lemon::ListDigraph::Node start , lemon::ListDigraph::Node end , QStringList route );

protected:

    QReadWriteLock mutex;

    // Used to create nodes and arcs
    lemon::ListDigraph* routing_graph = Q_NULLPTR;
    GWSEdgeVisitor* graph_edge_visitor = Q_NULLPTR;

    // To link arcs with its original GSSGraphEdge
    QMap< lemon::ListDigraph::Arc , QString >* arc_to_object_ids;

    // To link nodes with its original GSSGraphNode
    QMap< QString , lemon::ListDigraph::Node >* hash_to_node;

    // Routes cache
    QMap< lemon::ListDigraph::Node , QMap< lemon::ListDigraph::Node , QStringList > > routes_cache; // QMAP< DEPARTURE , QMAP< DESTINATION , ROUTE > >

};


#endif // GWSROUTING_H
