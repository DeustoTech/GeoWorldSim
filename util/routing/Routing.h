#ifndef ROUTING_H
#define ROUTING_H

#include <QObject>
#include <QReadWriteLock>
#include <QMap>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

#include "../../util/graph/Edge.h"
#include "../../util/routing/EdgeVisitor.hpp"

namespace geoworldsim{
namespace routing {


class Routing : public QObject
{
    Q_OBJECT

public:

    Routing();
    ~Routing();

    // GETTERS
    QStringList getShortestPath( const QString &from_hash , const QString &to_hash );
    QList< QStringList > getShortestPath( const QStringList &ordered_hashes );
    QList< QStringList > getShortestPaths( const QString &from_one_hash , const QStringList &to_many_hashes );

public slots:
    void upsertEdge(const QString &object_id , const graph::Edge &edge );
    void removeEdge( const QString &object_id );

signals:
    void upsertEdgeSignal(const QString &object_id , const graph::Edge &edge );
    void removeEdgeSignal( const QString &object_id );

protected:

    void cachePath( lemon::ListDigraph::Node start , lemon::ListDigraph::Node end , QStringList route );

protected:

    QReadWriteLock mutex;

    // Used to create nodes and arcs
    lemon::ListDigraph* routing_graph = Q_NULLPTR;
    graph::EdgeVisitor* graph_edge_visitor = Q_NULLPTR;

    // To link arcs with its original GSSGraphEdge
    QMap< lemon::ListDigraph::Arc , std::string >* arc_to_object_ids;

    // To link nodes with its original GSSGraphNode
    QMap< std::string , lemon::ListDigraph::Node >* hash_to_node;

    // Routes cache
    QMap< lemon::ListDigraph::Node , QMap< lemon::ListDigraph::Node , QStringList > > routes_cache; // QMAP< DEPARTURE , QMAP< DESTINATION , ROUTE > >

};

}
}

#endif // ROUTING_H
