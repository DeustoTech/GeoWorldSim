#ifndef GWSROUTING_H
#define GWSROUTING_H

#include <QImage>
#include "object/Object.h"

#include <lemon/core.h>
#include <lemon/list_graph.h>

#include <geos/geom/Geometry.h>
#include <geos/geom/Coordinate.h>

#include "util/routing/GraphEdgeArcMap.h"
#include "util/graph/GraphEdge.h"
#include "util/graph/GraphNode.h"

using namespace geos::geom;
using namespace lemon;

QT_FORWARD_DECLARE_CLASS(GWSGraphEdgeArcMap)

class GWSRouting : public GWSObject
{
    Q_OBJECT
    friend class GWSGraphEdgeArcMap;

public:
    GWSRouting( QList<GWSGraphEdge*> edges );
    ~GWSRouting();

    // EXPORTERS
    virtual QImage toImage( const GWSEnvelope image_bounds , unsigned int image_width = 1024 , unsigned int image_height = 1024 ) const;

    // GETTERS
    GWSGraphNode* findNearestNode( const GWSCoordinate coor );
    GWSGraphNode* getNodeFromNode( const ListDigraph::Node node );
    GWSGraphEdge* getEdgeFromArc( const ListDigraph::Arc arc );

protected:

    // Used to create nodes and arcs
    ListDigraph* routing_graph;

    // To link arcs with its original GSSGraphEdge
    QMap< ListDigraph::Arc , GWSGraphEdge*> arc_to_edges;

    // To link nodes with its original GSSGraphNode
    QMap< ListDigraph::Node , GWSGraphNode*> node_to_nodes;

private:

    GWSQuadtree* nodes_index; // In order to be able to find nearest nodes

};

#endif // GWSROUTING_H
