#ifndef GWSGRAPH_H
#define GWSGRAPH_H

#include "geos/planargraph/PlanarGraph.h"
#include "geos/geom/MultiLineString.h"

#include "../../util/geometry/Quadtree.h"
#include "../../util/graph/GraphNode.h"
#include "../../util/graph/GraphEdge.h"
#include "../../util/geometry/GeoCoordinates.h"

using namespace geos::planargraph;

class GWSGraph : public PlanarGraph
{
public:
    explicit GWSGraph();
    ~GWSGraph();

    // EXPORTERS
    virtual QImage toImage( const GWSEnvelope image_bounds , unsigned int image_width = 1024 , unsigned int image_height = 1024 ) const;

    // GETTERS
    bool containsNode( GWSGraphNode* node ) const;
    bool containsEdge( GWSGraphEdge* edge ) const;
    GWSGraphNode* findNode( GeoCoordinates coor ) const;
    GWSGraphNode* findNearestNode( GeoCoordinates coor ) const;
    GWSGraphEdge* findEdge( GeoCoordinates from , GeoCoordinates to ) const;
    QList<GWSGraphNode*> findNodesOfDegree( int degree ) const;
    QList<GWSGraphNode*> getNodes() const;
    QList<GWSGraphEdge*> getEdges() const;
    QList<GWSGraphEdge*> getEdgesInEnvelope( GWSEnvelope ) const;
    QMap<GWSGraphEdge*, double> getCostMap() const;
    int countNodes() const;
    int countEdges() const;
    //geos::geom::MultiLineString* toMultiLineString() const;

    // SETTERS
    void replaceNode( GWSGraphNode* old_node , GWSGraphNode* new_node );
    void addNode( GWSGraphNode* node );
    void addEdge( GWSGraphEdge* edge );
    void addGraph( const GWSGraph* other );
    void removeNode( GWSGraphNode* node );
    void removeEdge( GWSGraphEdge* edge );

private:
    GWSQuadtree* nodes_index;
};

#endif // GWSGRAPH_H
