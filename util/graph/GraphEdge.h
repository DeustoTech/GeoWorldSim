#ifndef GSSGRAPHEDGE_H
#define GSSGRAPHEDGE_H

#include "geos/planargraph/DirectedEdge.h"
#include "geos/planargraph/PlanarGraph.h"

#include "../../util/units/Units.h"

QT_FORWARD_DECLARE_CLASS(GWSGraphNode)

using namespace geos::planargraph;

class GWSGraphEdge
{

public:
    GWSGraphEdge();
    ~GWSGraphEdge();

    // GETTERS
    virtual GWSGraphNode* getFromNode() const;
    virtual GWSGraphNode* getToNode() const;
    virtual GWSLengthUnit getLength() const;
    virtual double getGradient() const; // Positive for climbing up and negative for going down
    virtual bool equals( const GWSGraphEdge* other) const;
    virtual bool equalsReversed( const GWSGraphEdge* other ) const;

    // SETTERS
    void setLength(GWSLengthUnit length);

    // METHODS
    virtual double getCost( double accumulated_cost = 0 ) const; // To be implemented by each child

private:

    GWSGraphNode* from = Q_NULLPTR;
    GWSGraphNode* to = Q_NULLPTR;
    GWSLengthUnit length = 0;
};

#endif // GSSGRAPHEDGE_H
