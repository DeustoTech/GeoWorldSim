#include "GraphEdge.h"

#include <QDebug>
#include "../../util/graph/GraphNode.h"

GWSGraphEdge::GWSGraphEdge() {
}

GWSGraphEdge::~GWSGraphEdge(){
    // Disconnect
    if( this->from ){ this->from->disconnectEdge( this ); }
    if( this->to ){ this->to->disconnectEdge( this ); }
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSGraphNode* GWSGraphEdge::getFromNode() const{
    return this->from;
}

GWSGraphNode* GWSGraphEdge::getToNode() const{
    return this->to;
}

GWSLengthUnit GWSGraphEdge::getLength() const {
    return this->length;
}

/**
 * @brief Road::getGradient Get the roads gradient
 * (positive for climbing up and negative for going down)
 * @return
 */
double GWSGraphEdge::getGradient() const{
    double length = this->getLength().number();
    if( length ){
        try {

            double height ( this->getFromNode()->getCoordinate().getZ() - this->getToNode()->getCoordinate().getZ() );
            if( height == height ){ // Avoid NaN
                return ( height - 100 / length );
            }
        } catch(...){}
    }
    return 0;
}

/**
 * @brief GSSGraphEdge::getCost
 * @param accumulated_cost Accumulated cost at which this edge was reached (used to know at what time we pass)
 * @return
 */
double GWSGraphEdge::getCost(double accumulated_cost) const{
    Q_UNUSED( accumulated_cost )
    return this->length.value;
}

bool GWSGraphEdge::equals( const GWSGraphEdge *other) const{
    return this->getFromNode()->equals( other->getFromNode() ) && this->getToNode()->equals( other->getToNode() );
}

bool GWSGraphEdge::equalsReversed(const GWSGraphEdge *other) const{
    return this->getFromNode()->equals( other->getToNode() ) && this->getToNode()->equals( other->getFromNode() );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGraphEdge::setLength(GWSLengthUnit length){
    this->length = length;
}
