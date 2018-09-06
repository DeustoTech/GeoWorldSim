#ifndef GWSGRAPH_H
#define GWSGRAPH_H

#include <QSharedPointer>

#include "../../util/geometry/Quadtree.h"
#include "../../util/graph/GraphNode.h"
#include "../../util/graph/GraphEdge.h"

class GWSGraph
{

public:
    explicit GWSGraph();
    ~GWSGraph();

    // EXPORTERS
    //virtual QImage toImage( const GWSEnvelope image_bounds , unsigned int image_width = 1024 , unsigned int image_height = 1024 ) const;

    // GETTERS
    bool containsNode( QSharedPointer<GWSGraphNode> node ) const;
    bool containsEdge( QSharedPointer<GWSGraphEdge> edge ) const;
    const QSharedPointer<GWSGraphNode> findNode( GWSCoordinate point ) const;
    const QSharedPointer<GWSGraphNode> findNearestNode( GWSCoordinate point ) const;
    const QSharedPointer<GWSGraphEdge> findEdge( GWSCoordinate from , GWSCoordinate to ) const;
    QList<QSharedPointer<GWSGraphNode> > findNodesOfDegree( int degree ) const;
    //QList<const QSharedPointer<GWSGraphNode>> getNodes() const;
    QList<QSharedPointer<GWSGraphEdge> > getEdges() const;
    QMap<QSharedPointer<GWSGraphEdge>, double> getCostMap() const;
    int countNodes() const;
    int countEdges() const;

    // SETTERS
    void addNode( QSharedPointer<GWSGraphNode> node );
    void addEdge( QSharedPointer<GWSGraphEdge> edge );
    void addGraph( const GWSGraph* other );
    void removeNode( QSharedPointer<GWSGraphNode> node );
    void removeEdge( QSharedPointer<GWSGraphEdge> edge );

private:
    GWSQuadtree* nodes_index;
    QList< QSharedPointer<GWSGraphNode> > nodes;
    QList< QSharedPointer<GWSGraphEdge> > edges;
};

#endif // GWSGRAPH_H
