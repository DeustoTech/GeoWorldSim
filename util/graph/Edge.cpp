#include "Edge.h"

/**********************************************************************
 GETTERS
**********************************************************************/

/*QString GWSEdge::getUID() const{
    return this->type + "::" + this->getFromNodeUID() + "->" + this->getToNodeUID();
}*/

bool GWSEdge::isValid() const{
    return !this->getFromNodeUID().isEmpty() && !this->getToNodeUID().isEmpty();
}

double GWSEdge::getCost( double accumulated_cost ) const{
    return this->cost + accumulated_cost;
}

QString GWSEdge::getType() const{
    return this->type;
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool GWSEdge::operator == (const GWSEdge& other) const {
    return ( this->getFromNodeUID() == other.getFromNodeUID() ) && ( this->getToNodeUID() == other.getToNodeUID() );
}

bool GWSEdge::operator != (const GWSEdge& other) const{
    return !(*this == other);
}

bool GWSEdge::operator < (const GWSEdge& other) const{
    return this->getFromNodeUID() < other.getFromNodeUID();
}

GWSEdge& GWSEdge::operator = (const GWSEdge& other){
    this->cost = other.cost;
    this->type = other.type;
    return *this;
}
