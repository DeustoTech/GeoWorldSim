#include "GraphEdge.h"

#include <QDebug>
#include "../../environment/network_environment/NetworkEnvironment.h"

QString GWSGraphEdge::EDGE_FROM_X_PROP = "from_x";
QString GWSGraphEdge::EDGE_FROM_Y_PROP = "from_y";
QString GWSGraphEdge::EDGE_FROM_Z_PROP = "from_z";
QString GWSGraphEdge::EDGE_TO_X_PROP = "to_x";
QString GWSGraphEdge::EDGE_TO_Y_PROP = "to_y";
QString GWSGraphEdge::EDGE_TO_Z_PROP = "to_z";

GWSGraphEdge::GWSGraphEdge() : GWSObject() {
}

GWSGraphEdge::~GWSGraphEdge(){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSGraphEdge::deserialize(QJsonObject json, QSharedPointer<GWSObject> parent){
    GWSObject::deserialize( json , parent );
    this->length = GWSLengthUnit( this->getFrom().getDistance( this->getTo() ) );
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSGraphEdge::serialize() const{
    QJsonObject json = GWSObject::serialize();
    return json;
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSCoordinate GWSGraphEdge::getFrom() const{
    return GWSCoordinate( this->getProperty( EDGE_FROM_X_PROP ).toDouble() , this->getProperty( EDGE_FROM_Y_PROP ).toDouble() , this->getProperty( EDGE_FROM_Z_PROP ).toDouble() );
}

GWSCoordinate GWSGraphEdge::getTo() const{
    return GWSCoordinate( this->getProperty( EDGE_TO_X_PROP ).toDouble() , this->getProperty( EDGE_TO_Y_PROP ).toDouble() , this->getProperty( EDGE_TO_Z_PROP ).toDouble() );
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
double GWSGraphEdge::getCost(double accumulated_cost) const{
    Q_UNUSED( accumulated_cost )
    return this->length.value;
}

bool GWSGraphEdge::equals( const QSharedPointer<GWSGraphEdge> other) const{
    return this->getFrom() == other->getFrom() && this->getTo() == other->getTo();
}

bool GWSGraphEdge::equalsReversed(const QSharedPointer<GWSGraphEdge> other) const{
    return this->getFrom() == other->getTo() && this->getTo() == other->getFrom();
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGraphEdge::setLength(GWSLengthUnit length){
    this->length = length;
}
