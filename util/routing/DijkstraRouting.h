#ifndef GWSDIJKSTRAROUTING_H
#define GWSDIJKSTRAROUTING_H

#include <QMutex>

#include "util/routing/Routing.h"

#include <lemon/dijkstra.h>

/**
 * @brief The DijkstraRouting class
 */
class GWSDijkstraRouting : public GWSRouting
{
    Q_OBJECT

public:
    GWSDijkstraRouting( QList<GWSGraphEdge*> edges );
    ~GWSDijkstraRouting();

    // METHODS
    QList<GWSGraphEdge*> dijkstraShortestPath(GWSGraphNode* from, GWSGraphNode* to );
    QList<QList<GWSGraphEdge*> > dijkstraShortestPath(QList<GWSGraphNode*> ordered_nodes );
    QList<QList<GWSGraphEdge*> > dijkstraShortestPaths(GWSGraphNode* from_node, QList<GWSGraphNode*> to_nodes );
    GWSGraphNode* dijkstraNearestNode(GWSGraphNode* from_node, QList<GWSGraphNode*> to_nodes );

private:
    QMutex mutex;
};

#endif // GWSDIJKSTRAROUTING_H
