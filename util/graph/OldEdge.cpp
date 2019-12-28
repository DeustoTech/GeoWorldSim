#include "OldEdge.h"

GWSOldEdge::GWSOldEdge() : GWSObject(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

/**
 * @brief GWSEdge::getCost
 * @param accumulated_cost Accumulated cost at which this edge was reached (used to know at what time reached or length)
 * @return
 */
double GWSOldEdge::getEdgeCost( double accumulated_cost ) const{
    Q_UNUSED( accumulated_cost );
    return this->cost;
}

QString GWSOldEdge::getEdgeType() const{
    return this->type;
}

bool GWSOldEdge::equals( const QSharedPointer<GWSOldEdge> other ) const{
    return this->getFromNodeId() == other->getFromNodeId() && this->getToNodeId() == other->getToNodeId();
}

bool GWSOldEdge::equalsReversed(const QSharedPointer<GWSOldEdge> other) const{
    return this->getFromNodeId() == other->getToNodeId() && this->getToNodeId() == other->getFromNodeId();
}

