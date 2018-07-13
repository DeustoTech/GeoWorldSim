#ifndef GSSGRAPHNODE_H
#define GSSGRAPHNODE_H

#include <QList>

#include "geos/geom/Coordinate.h"
#include "geos/planargraph/Node.h"
#include "geos/planargraph/PlanarGraph.h"

#include "../../util/geometry/GeoCoordinates.h"
#include "../../util/graph/Graph.h"

QT_FORWARD_DECLARE_CLASS(GWSGraphEdge)

using namespace geos::geom;
using namespace geos::planargraph;

class GWSGraphNode : public geos::planargraph::Node
{
    friend class GWSGraph;
    friend class GWSGraphEdge;

public:
    GWSGraphNode( GeoCoordinates coor );

    // GETTERS
    virtual const QList<GWSGraphEdge*> getDepartingEdges() const;
    virtual const QList<GWSGraphEdge*> getArrivingEdges() const;
    virtual const GWSGraphEdge* getDepartingEdgeTo( GWSGraphNode* to ) const;
    virtual const GWSGraphEdge* getArrivingEdgeFrom( GWSGraphNode* from ) const;
    virtual const GeoCoordinates getCoordinate() const;
    virtual size_t getDegree() const;

    // SETTERS

    // METHODS
    virtual double getCost() const; // To be implemented by each child
    bool equals(GWSGraphNode* other);

private:
    void connectEdge(GWSGraphEdge* edge);
    void disconnectEdge(GWSGraphEdge* edge);

    QList<GWSGraphEdge*> out_edges; // Departing edges
    QList<GWSGraphEdge*> in_edges; // Arriving edges
};

#endif // GSSGRAPHNODE_H
