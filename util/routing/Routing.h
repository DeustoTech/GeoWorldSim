#ifndef GWSROUTING_H
#define GWSROUTING_H

#include <QMutex>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

#include "../../util/graph/GraphEdge.h"
#include "../../util/routing/GraphEdgeVisitor.h"

class GWSRouting : public GWSObject
{
    Q_OBJECT
    friend class GWSGraphEdgeVisitor;

public:
    GWSRouting();
    ~GWSRouting();

    // GETTERS
    QList<QSharedPointer<GWSGraphEdge> > getShortestPath( GWSCoordinate from, GWSCoordinate to );
    QList<QList<QSharedPointer<GWSGraphEdge> > > getShortestPath(QList< GWSCoordinate > ordered_coors );
    QList<QList<QSharedPointer< GWSGraphEdge> > > getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many );

    // SETTERS
    void upsert( QSharedPointer<GWSGraphEdge> edge );
    void remove( QSharedPointer<GWSGraphEdge> edge );

protected:

    QMutex mutex;

    // Used to create nodes and arcs
    lemon::ListDigraph* routing_graph = Q_NULLPTR;
    lemon::Dijkstra< lemon::ListDigraph, GWSGraphEdgeVisitor >* dijkstra_algorithm = Q_NULLPTR;
    GWSGraphEdgeVisitor* graph_edge_visitor = Q_NULLPTR;

    // To link arcs with its original GSSGraphEdge
    QMap< lemon::ListDigraph::Arc , QSharedPointer<GWSGraphEdge> >* arc_to_edges;

    // To link nodes with its original GSSGraphNode
    QMap< GWSCoordinate , lemon::ListDigraph::Node >* coors_to_node;

    // Routes cache
    void cachePath( lemon::ListDigraph::Node start , lemon::ListDigraph::Node end , QList<QSharedPointer<GWSGraphEdge> > route = QList<QSharedPointer<GWSGraphEdge> >() );
    QMap< lemon::ListDigraph::Node , QMap< lemon::ListDigraph::Node , QList<QSharedPointer<GWSGraphEdge> > > > routes_cache; // QMAP< DEPARTURE , QMAP< DESTINATION , ROUTE > >

};

#endif // GWSROUTING_H
