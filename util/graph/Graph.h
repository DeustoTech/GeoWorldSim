#ifndef GWSGRAPH_H
#define GWSGRAPH_H


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
    bool containsNode( GWSGraphNode* node ) const;
    bool containsEdge( GWSGraphEdge* edge ) const;
    const GWSGraphNode* findNode( GWSCoordinate point ) const;
    const GWSGraphNode* findNearestNode( GWSCoordinate point ) const;
    const GWSGraphEdge* findEdge( GWSCoordinate from , GWSCoordinate to ) const;
    QList<const GWSGraphNode*> findNodesOfDegree( int degree ) const;
    //QList<const GWSGraphNode*> getNodes() const;
    //QList<const GWSGraphEdge*> getEdges() const;
    QMap<const GWSGraphEdge*, double> getCostMap() const;
    int countNodes() const;
    int countEdges() const;

    // SETTERS
    void addNode( GWSGraphNode* node );
    void addEdge( GWSGraphEdge* edge );
    void addGraph( const GWSGraph* other );
    void removeNode( GWSGraphNode* node );
    void removeEdge( GWSGraphEdge* edge );

private:
    GWSQuadtree* nodes_index;
    QList<GWSGraphNode*> nodes;
    QList<GWSGraphEdge*> edges;
};

#endif // GWSGRAPH_H
