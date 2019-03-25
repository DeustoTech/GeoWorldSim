#include "NewNetworkEdge.h"

QString GWSNewNetworkEdge::EDGE_FROM_X_PROP = "from_x";
QString GWSNewNetworkEdge::EDGE_FROM_Y_PROP = "from_y";
QString GWSNewNetworkEdge::EDGE_FROM_Z_PROP = "from_z";
QString GWSNewNetworkEdge::EDGE_TO_X_PROP = "to_x";
QString GWSNewNetworkEdge::EDGE_TO_Y_PROP = "to_y";
QString GWSNewNetworkEdge::EDGE_TO_Z_PROP = "to_z";

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSNewNetworkEdge::getFromNodeUID() const{
    return this->getFromCoordinate().toString();
}

QString GWSNewNetworkEdge::getToNodeUID() const{
    return this->getToCoordinate().toString();
}

GWSCoordinate GWSNewNetworkEdge::getFromCoordinate() const{
    return this->from;
}

GWSCoordinate GWSNewNetworkEdge::getToCoordinate() const{
    return this->to;
}

GWSLengthUnit GWSNewNetworkEdge::getLength() const {
    return GWSLengthUnit( this->cost );
}

/**
 * @brief GWSNewNetworkEdge::getGradient Get the roads gradient
 * (positive for climbing up and negative for going down)
 * @return
 */
double GWSNewNetworkEdge::getGradient() const{
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

void GWSNewNetworkEdge::setLength(GWSLengthUnit length){
    this->cost = length.number();
}
