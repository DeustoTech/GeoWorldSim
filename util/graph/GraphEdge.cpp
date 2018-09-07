#include "GraphEdge.h"

#include <QDebug>
#include "../../util/graph/GraphNode.h"

#include "../../environment/network_environment/NetworkEnvironment.h"

QString GWSGraphEdge::EDGE_FROM_X_PROP = "edge_from_x";
QString GWSGraphEdge::EDGE_FROM_Y_PROP = "edge_from_y";
QString GWSGraphEdge::EDGE_TO_X_PROP = "edge_to_x";
QString GWSGraphEdge::EDGE_TO_Y_PROP = "edge_to_y";

GWSGraphEdge::GWSGraphEdge() : GWSObject() {
}

GWSGraphEdge::~GWSGraphEdge(){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSGraphEdge::deserialize(QJsonObject json, QSharedPointer<GWSObject> parent){
    GWSObject::deserialize( json , parent );

    if( json.keys().contains( EDGE_FROM_X_PROP ) && json.keys().contains( EDGE_FROM_Y_PROP ) ){
        this->from = GWSCoordinate( json.value( EDGE_FROM_X_PROP ).toDouble() , json.value( EDGE_FROM_Y_PROP ).toDouble() );
    }

    if( json.keys().contains( EDGE_TO_X_PROP ) && json.keys().contains( EDGE_TO_Y_PROP ) ){
        this->to = GWSCoordinate( json.value( EDGE_TO_X_PROP ).toDouble() , json.value( EDGE_TO_Y_PROP ).toDouble() );
    }

    if( this->from.isValid() && this->to.isValid() ){
        this->length = GWSLengthUnit( this->from.getDistance( this->to ) );
    }

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
    return this->from;
}

GWSCoordinate GWSGraphEdge::getTo() const{
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

            double height ( this->from.getZ() - this->to.getZ() );
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
    return this->from == other->from && this->to == other->to;
}

bool GWSGraphEdge::equalsReversed(const QSharedPointer<GWSGraphEdge> other) const{
    return this->from == other->to && this->to == other->from;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGraphEdge::setLength(GWSLengthUnit length){
    this->length = length;
}
