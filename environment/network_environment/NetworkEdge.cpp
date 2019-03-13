#include "NetworkEdge.h"

#include <QDebug>
#include "../../environment/network_environment/NetworkEnvironment.h"

QString GWSNetworkEdge::EDGE_FROM_X_PROP = "from_x";
QString GWSNetworkEdge::EDGE_FROM_Y_PROP = "from_y";
QString GWSNetworkEdge::EDGE_FROM_Z_PROP = "from_z";
QString GWSNetworkEdge::EDGE_TO_X_PROP = "to_x";
QString GWSNetworkEdge::EDGE_TO_Y_PROP = "to_y";
QString GWSNetworkEdge::EDGE_TO_Z_PROP = "to_z";

GWSNetworkEdge::GWSNetworkEdge() : GWSEdge() {
}

GWSNetworkEdge::~GWSNetworkEdge(){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSNetworkEdge::deserialize(QJsonObject json, QSharedPointer<GWSObject> parent){
    GWSObject::deserialize( json , parent );
    this->cost = this->getFromCoordinate().getDistance( this->getToCoordinate() ).number();
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSNetworkEdge::getFromNodeId() const{
    return this->getFromCoordinate().toString();
}

QString GWSNetworkEdge::getToNodeId() const{
    return this->getToCoordinate().toString();
}

GWSCoordinate GWSNetworkEdge::getFromCoordinate() const{
    return GWSCoordinate( this->getProperty( EDGE_FROM_X_PROP ).toDouble() , this->getProperty( EDGE_FROM_Y_PROP ).toDouble() , this->getProperty( EDGE_FROM_Z_PROP ).toDouble() );
}

GWSCoordinate GWSNetworkEdge::getToCoordinate() const{
    return GWSCoordinate( this->getProperty( EDGE_TO_X_PROP ).toDouble() , this->getProperty( EDGE_TO_Y_PROP ).toDouble() , this->getProperty( EDGE_TO_Z_PROP ).toDouble() );
}

GWSLengthUnit GWSNetworkEdge::getLength() const {
    return GWSLengthUnit( this->cost );
}

/**
 * @brief Road::getGradient Get the roads gradient
 * (positive for climbing up and negative for going down)
 * @return
 */
double GWSNetworkEdge::getGradient() const{
    double length = this->getLength().number();
    if( length ){
        try {

            double height ( this->getFromCoordinate().getZ() - this->getToCoordinate().getZ() );
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
