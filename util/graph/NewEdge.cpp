#include "NewEdge.h"


/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSNewEdge::getUID() const{
    return this->getFromNodeUID() + this->getToNodeUID();
}

bool GWSNewEdge::isValid() const{
    return !this->getFromNodeUID().isEmpty() && !this->getToNodeUID().isEmpty();
}

double GWSNewEdge::getCost( double accumulated_cost ) const{
    return this->cost + accumulated_cost;
}

QString GWSNewEdge::getType() const{
    return this->type;
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool GWSNewEdge::operator == (const GWSNewEdge& other) const {
    return ( this->getFromNodeUID() == other.getFromNodeUID() ) && ( this->getToNodeUID() == other.getToNodeUID() );
}

bool GWSNewEdge::operator != (const GWSNewEdge& other) const{
    return !(*this == other);
}


bool GWSNewEdge::operator < (const GWSNewEdge& other) const{
    return this->getFromNodeUID() < other.getFromNodeUID();
}

GWSNewEdge& GWSNewEdge::operator = (const GWSNewEdge& other){
    this->cost = other.cost;
    this->type = other.type;
    return *this;
}
