#include "Edge.h"

/**********************************************************************
 GETTERS
**********************************************************************/

/*QString geoworldsim::graph::GWSEdge::getUID() const{
    return this->type + "::" + this->getFromNodeUID() + "->" + this->getToNodeUID();
}*/

bool geoworldsim::graph::Edge::isValid() const{
    return !this->getFromNodeUID().isEmpty() && !this->getToNodeUID().isEmpty();
}

double geoworldsim::graph::Edge::getCost( double accumulated_cost ) const{
    return this->cost + accumulated_cost;
}

QString geoworldsim::graph::Edge::getType() const{
    return this->type;
}

/**********************************************************************
 OPERATORS
**********************************************************************/

bool geoworldsim::graph::Edge::operator == (const geoworldsim::graph::Edge& other) const {
    return ( this->getFromNodeUID() == other.getFromNodeUID() ) && ( this->getToNodeUID() == other.getToNodeUID() );
}

bool geoworldsim::graph::Edge::operator != (const geoworldsim::graph::Edge& other) const{
    return !(*this == other);
}

bool geoworldsim::graph::Edge::operator < (const geoworldsim::graph::Edge& other) const{
    return this->getFromNodeUID() < other.getFromNodeUID();
}

geoworldsim::graph::Edge& geoworldsim::graph::Edge::operator = (const geoworldsim::graph::Edge& other){
    this->cost = other.cost;
    this->type = other.type;
    return *this;
}
