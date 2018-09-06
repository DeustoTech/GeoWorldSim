#ifndef GWSDIJKSTRAROUTING_H
#define GWSDIJKSTRAROUTING_H

#include <QMutex>

#include "../../util/routing/Routing.h"

#include <lemon/dijkstra.h>

/**
 * @brief The DijkstraRouting class
 */
class GWSDijkstraRouting : public GWSRouting
{
    Q_OBJECT

public:
    GWSDijkstraRouting( QList<QSharedPointer<GWSGraphEdge> > edges );
    ~GWSDijkstraRouting();

    // METHODS
    QList<QSharedPointer<GWSGraphEdge> > dijkstraShortestPath(QSharedPointer<GWSGraphNode> from, QSharedPointer<GWSGraphNode> to );
    QList<QList<QSharedPointer<GWSGraphEdge> > > dijkstraShortestPath(QList<QSharedPointer<GWSGraphNode> > ordered_nodes );
    QList<QList<QSharedPointer< GWSGraphEdge> > > dijkstraShortestPaths(QSharedPointer<GWSGraphNode> from_node, QList<QSharedPointer<GWSGraphNode> > to_nodes );
    QSharedPointer<GWSGraphNode> dijkstraNearestNode(QSharedPointer<GWSGraphNode> from_node, QList<QSharedPointer<GWSGraphNode> > to_nodes );

private:
    QMutex mutex;
};

#endif // GWSDIJKSTRAROUTING_H
