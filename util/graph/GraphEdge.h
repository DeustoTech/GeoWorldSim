#ifndef GWSGRAPHEDGE_H
#define GWSGRAPHEDGE_H

#include "geos/planargraph/DirectedEdge.h"
#include "geos/planargraph/PlanarGraph.h"

#include "../../util/units/Units.h"

QT_FORWARD_DECLARE_CLASS(GWSAgent)
QT_FORWARD_DECLARE_CLASS(GWSGraphNode)

using namespace geos::planargraph;

class GWSGraphEdge
{

public:
    GWSGraphEdge( QSharedPointer<GWSAgent> agent );
    ~GWSGraphEdge();

    // GETTERS
    virtual QSharedPointer<GWSGraphNode> getFromNode() const;
    virtual QSharedPointer<GWSGraphNode> getToNode() const;
    virtual GWSLengthUnit getLength() const;
    virtual double getGradient() const; // Positive for climbing up and negative for going down
    virtual bool equals( const QSharedPointer<GWSGraphEdge> other) const;
    virtual bool equalsReversed( const QSharedPointer<GWSGraphEdge> other ) const;

    // SETTERS
    void setLength(GWSLengthUnit length);

    // METHODS
    virtual double getCost( double accumulated_cost = 0 ) const; // To be implemented by each child

private:

    // REF AGENT
    QSharedPointer<GWSAgent> agent;

    QSharedPointer<GWSGraphNode> from;
    QSharedPointer<GWSGraphNode> to;
    GWSLengthUnit length = 0;
};

#endif // GWSGRAPHEDGE_H
