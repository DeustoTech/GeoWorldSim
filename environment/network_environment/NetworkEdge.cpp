#include "NetworkEdge.h"

QString geoworldsim::graph::NetworkEdge::EDGE_FROM_X_PROP = "from_x";
QString geoworldsim::graph::NetworkEdge::EDGE_FROM_Y_PROP = "from_y";
QString geoworldsim::graph::NetworkEdge::EDGE_FROM_Z_PROP = "from_z";
QString geoworldsim::graph::NetworkEdge::EDGE_TO_X_PROP = "to_x";
QString geoworldsim::graph::NetworkEdge::EDGE_TO_Y_PROP = "to_y";
QString geoworldsim::graph::NetworkEdge::EDGE_TO_Z_PROP = "to_z";

/**********************************************************************
 GETTERS
**********************************************************************/

bool geoworldsim::graph::NetworkEdge::isValid() const {
    return this->getFromCoordinate().isValid() && this->getToCoordinate().isValid();
}

geoworldsim::geometry::Coordinate geoworldsim::graph::NetworkEdge::getFromCoordinate() const{
    return this->from;
}

geoworldsim::geometry::Coordinate geoworldsim::graph::NetworkEdge::getToCoordinate() const{
    return this->to;
}

geoworldsim::unit::LengthUnit geoworldsim::graph::NetworkEdge::getLength() const {
    return unit::LengthUnit( this->cost );
}

/**
 * @brief GWSNewNetworkEdge::getGradient Get the roads gradient
 * (positive for climbing up and negative for going down)
 * @return
 */
double geoworldsim::graph::NetworkEdge::getGradient() const{
    double length = this->getLength().number();
    if( length ){
        try {
            double height ( this->from.getZ() - this->to.getZ() );
            if( height == height ){ // Avoid NaN
                return ( height - 100 / length );
            }
        } catch(...){}
    }
    return 0;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void geoworldsim::graph::NetworkEdge::setLength(geoworldsim::unit::LengthUnit length){
    this->cost = length.number();
}
