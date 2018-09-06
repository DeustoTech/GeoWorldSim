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
    // Disconnect
    if( this->from ){ this->from->disconnectEdge( this->getSharedPointer().dynamicCast<GWSGraphEdge>() ); }
    if( this->to ){ this->to->disconnectEdge( this->getSharedPointer().dynamicCast<GWSGraphEdge>() ); }
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSGraphEdge::deserialize(QJsonObject json, QSharedPointer<GWSObject> parent){
    GWSObject::deserialize( json , parent );

    if( json.keys().contains( EDGE_FROM_X_PROP ) && json.keys().contains( EDGE_FROM_Y_PROP ) ){

        GWSCoordinate coor = GWSCoordinate( json.value( EDGE_FROM_X_PROP ).toDouble() , json.value( EDGE_FROM_Y_PROP ).toDouble() );
        QSharedPointer<GWSGraphNode> from_node = GWSNetworkEnvironment::globalInstance()->getNodeFromGraph( coor , GWSAgent::staticMetaObject.className() );
        if( !from_node ){
            from_node = QSharedPointer<GWSGraphNode>( new GWSGraphNode() );
            from_node->inner_coordinate = coor;
        }
        this->from = from_node;
    }

    if( json.keys().contains( EDGE_TO_X_PROP ) && json.keys().contains( EDGE_TO_Y_PROP ) ){

        GWSCoordinate coor = GWSCoordinate( json.value( EDGE_TO_X_PROP ).toDouble() , json.value( EDGE_TO_Y_PROP ).toDouble() );
        QSharedPointer<GWSGraphNode> to_node = GWSNetworkEnvironment::globalInstance()->getNodeFromGraph( coor , GWSAgent::staticMetaObject.className() );
        if( !to_node ){
            to_node = QSharedPointer<GWSGraphNode>( new GWSGraphNode() );
            to_node->inner_coordinate = coor;
        }
        this->to = to_node;
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

QSharedPointer<GWSGraphNode> GWSGraphEdge::getFromNode() const{
    return this->from;
}

QSharedPointer<GWSGraphNode> GWSGraphEdge::getToNode() const{
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

            double height ( this->getFromNode()->getCoordinate().getZ() - this->getToNode()->getCoordinate().getZ() );
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
    return this->getFromNode()->equals( other->getFromNode() ) && this->getToNode()->equals( other->getToNode() );
}

bool GWSGraphEdge::equalsReversed(const QSharedPointer<GWSGraphEdge> other) const{
    return this->getFromNode()->equals( other->getToNode() ) && this->getToNode()->equals( other->getFromNode() );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGraphEdge::setLength(GWSLengthUnit length){
    this->length = length;
}
