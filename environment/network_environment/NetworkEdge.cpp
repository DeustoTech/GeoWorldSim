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
    this->length = GWSLengthUnit( this->getFrom().getDistance( this->getTo() ) );
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSCoordinate GWSNetworkEdge::getFrom() const{
    return GWSCoordinate( this->getProperty( EDGE_FROM_X_PROP ).toDouble() , this->getProperty( EDGE_FROM_Y_PROP ).toDouble() , this->getProperty( EDGE_FROM_Z_PROP ).toDouble() );
}

GWSCoordinate GWSNetworkEdge::getTo() const{
    return GWSCoordinate( this->getProperty( EDGE_TO_X_PROP ).toDouble() , this->getProperty( EDGE_TO_Y_PROP ).toDouble() , this->getProperty( EDGE_TO_Z_PROP ).toDouble() );
}

GWSLengthUnit GWSNetworkEdge::getLength() const {
    return this->length;
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

            double height ( this->getFrom().getZ() - this->getTo().getZ() );
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
double GWSNetworkEdge::getCost(double accumulated_cost) const{
    Q_UNUSED( accumulated_cost )
    return this->length.value;
}

bool GWSNetworkEdge::equals( const QSharedPointer<GWSNetworkEdge> other) const{
    return this->getFrom() == other->getFrom() && this->getTo() == other->getTo();
}

bool GWSNetworkEdge::equalsReversed(const QSharedPointer<GWSNetworkEdge> other) const{
    return this->getFrom() == other->getTo() && this->getTo() == other->getFrom();
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSNetworkEdge::setLength(GWSLengthUnit length){
    this->length = length;
}
