#include "Edge.h"

GWSEdge::GWSEdge() : GWSObject(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

/**
 * @brief GWSEdge::getCost
 * @param accumulated_cost Accumulated cost at which this edge was reached (used to know at what time reached or length)
 * @return
 */
double GWSEdge::getEdgeCost( double accumulated_cost ) const{
    Q_UNUSED( accumulated_cost );
    return this->cost;
}

QString GWSEdge::getEdgeType() const{
    return this->type;
}

bool GWSEdge::equals( const QSharedPointer<GWSEdge> other ) const{
    return this->getFromNodeId() == other->getFromNodeId() && this->getToNodeId() == other->getToNodeId();
}

bool GWSEdge::equalsReversed(const QSharedPointer<GWSEdge> other) const{
    return this->getFromNodeId() == other->getToNodeId() && this->getToNodeId() == other->getFromNodeId();
}
