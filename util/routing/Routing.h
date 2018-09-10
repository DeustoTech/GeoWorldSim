#ifndef GWSROUTING_H
#define GWSROUTING_H

#include <QImage>
#include "../../object/Object.h"
#include "../../util/geometry/Quadtree.h"

#include <lemon/core.h>
#include <lemon/list_graph.h>

#include "../../util/routing/GraphEdgeArcMap.h"
#include "../../util/graph/GraphEdge.h"
#include "../../util/graph/GraphNode.h"

using namespace lemon;

QT_FORWARD_DECLARE_CLASS(GWSGraphEdgeArcMap)

class GWSRouting : public GWSObject
{
    Q_OBJECT
    friend class GWSGraphEdgeArcMap;

public:
    explicit GWSRouting( QList< QSharedPointer<GWSGraphEdge> > edges );
    ~GWSRouting();

    // EXPORTERS
    //virtual QImage toImage( const GWSEnvelope image_bounds , unsigned int image_width = 1024 , unsigned int image_height = 1024 ) const;

    // GETTERS
    GWSCoordinate findNearestRoutingCoordinate( const GWSCoordinate coor );
    //QSharedPointer<GWSGraphNode> getNodeFromNode( const ListDigraph::Node node );
    //QSharedPointer<GWSGraphEdge> getEdgeFromArc( const ListDigraph::Arc arc );

    class GWSRoutingNode : public GWSObject {
    public:
        GWSRoutingNode( ListDigraph::Node ref ) : referenced_lemon_node(ref){};
        ListDigraph::Node referenced_lemon_node;
    };

protected:

    // Used to create nodes and arcs
    ListDigraph* routing_graph;

    // To link arcs with its original GSSGraphEdge
    QMap< ListDigraph::Arc , QSharedPointer<GWSGraphEdge> > arc_to_edges;

    // To link nodes with its original GSSGraphNode
    QMap< GWSCoordinate , ListDigraph::Node > coors_to_node;

private:

    GWSQuadtree* nodes_index = Q_NULLPTR; // In order to be able to find nearest nodes

};

#endif // GWSROUTING_H
