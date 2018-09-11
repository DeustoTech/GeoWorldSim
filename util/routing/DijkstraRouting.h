#ifndef GWSDIJKSTRAROUTING_H
#define GWSDIJKSTRAROUTING_H

#include <QMutex>

#include "../../util/routing/Routing.h"

#include <lemon/dijkstra.h>

QT_FORWARD_DECLARE_CLASS(GWSGraphEdgeArcMap)

/**
 * @brief The DijkstraRouting class
 */
class GWSDijkstraRouting : public GWSRouting
{
    Q_OBJECT
    friend class GWSGraphEdgeArcMap;

public:
    GWSDijkstraRouting( QList<QSharedPointer<GWSGraphEdge> > edges );
    ~GWSDijkstraRouting();

    // METHODS
    void generateGraph();
    QList<QSharedPointer<GWSGraphEdge> > dijkstraShortestPath( GWSCoordinate from, GWSCoordinate to );
    QList<QList<QSharedPointer<GWSGraphEdge> > > dijkstraShortestPath(QList< GWSCoordinate > ordered_coors );
    QList<QList<QSharedPointer< GWSGraphEdge> > > dijkstraShortestPaths( GWSCoordinate from, QList< GWSCoordinate > to );
    //QSharedPointer<GWSGraphNode> dijkstraNearestNode( GWSCoordinate from_node, QList< GWSCoordinate > to_nodes );

private:
    QMutex mutex;
    Dijkstra<ListDigraph, GWSGraphEdgeArcMap >* dijkstra_algorithm = Q_NULLPTR;
    GWSGraphEdgeArcMap* routing_graph_costs = Q_NULLPTR;
};

#endif // GWSDIJKSTRAROUTING_H
