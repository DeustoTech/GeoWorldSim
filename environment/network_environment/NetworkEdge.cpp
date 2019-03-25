#include "NetworkEdge.h"

QString GWSNetworkEdge::EDGE_FROM_X_PROP = "from_x";
QString GWSNetworkEdge::EDGE_FROM_Y_PROP = "from_y";
QString GWSNetworkEdge::EDGE_FROM_Z_PROP = "from_z";
QString GWSNetworkEdge::EDGE_TO_X_PROP = "to_x";
QString GWSNetworkEdge::EDGE_TO_Y_PROP = "to_y";
QString GWSNetworkEdge::EDGE_TO_Z_PROP = "to_z";

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSNetworkEdge::getFromNodeUID() const{
    return this->getFromCoordinate().toString();
}

QString GWSNetworkEdge::getToNodeUID() const{
    return this->getToCoordinate().toString();
}

GWSCoordinate GWSNetworkEdge::getFromCoordinate() const{
    return this->from;
}

GWSCoordinate GWSNetworkEdge::getToCoordinate() const{
    return this->to;
}

GWSLengthUnit GWSNetworkEdge::getLength() const {
    return GWSLengthUnit( this->cost );
}

/**
 * @brief GWSNewNetworkEdge::getGradient Get the roads gradient
 * (positive for climbing up and negative for going down)
 * @return
 */
double GWSNetworkEdge::getGradient() const{
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

void GWSNetworkEdge::setLength(GWSLengthUnit length){
    this->cost = length.number();
}
