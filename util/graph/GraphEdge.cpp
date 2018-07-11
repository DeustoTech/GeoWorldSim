#include "GraphEdge.h"

#include <QDebug>
#include "../../util/graph/GraphNode.h"

/**
 * WARNING!! IF START NODE AND END NODE ARE THE SAME, CREATING A GRAPHEDGE WILL CRASH
 */
GWSGraphEdge::GWSGraphEdge(GWSGraphNode* start_node, GWSGraphNode* end_node) : geos::planargraph::DirectedEdge( start_node , end_node , geos::geom::Coordinate( end_node->getCoordinate().getX() , end_node->getCoordinate().getY() , end_node->getCoordinate().getZ() ) , true ){
    if( !start_node || !end_node ){
        qWarning() << Q_FUNC_INFO << "Edge created without start or end node";
        return;
    }
    this->from = start_node;
    this->to = end_node;

    // Connect
    dynamic_cast<GWSGraphNode*>( this->from )->connectEdge( this );
    dynamic_cast<GWSGraphNode*>( this->to )->connectEdge( this );

    // Magic number 110574
    this->length = GWSLengthUnit( this->from->getCoordinate().distance( this->to->getCoordinate() ) * 110574 );
}

GWSGraphEdge::GWSGraphEdge(GWSGraphEdge *first_edge, GWSGraphEdge *second_edge) : GWSGraphEdge( first_edge->getFromNode() , second_edge->getToNode() ){
    this->setLength( first_edge->getLength() + second_edge->getLength() );
}

GWSGraphEdge::~GWSGraphEdge(){
    // Disconnect
    dynamic_cast<GWSGraphNode*>( this->from )->disconnectEdge( this );
    dynamic_cast<GWSGraphNode*>( this->to )->disconnectEdge( this );
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSGraphNode* GWSGraphEdge::getFromNode() const{
    return dynamic_cast<GWSGraphNode*>( geos::planargraph::DirectedEdge::getFromNode() );
}

GWSGraphNode* GWSGraphEdge::getToNode() const{
    return dynamic_cast<GWSGraphNode*>( geos::planargraph::DirectedEdge::getToNode() );
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
    if( this->getLength().number() >= 0 ){
        try {

            double height ( this->getFromNode()->getCoordinate().getZ() - this->getToNode()->getCoordinate().getZ() );
            if( height == height ){ // Avoid NaN
                return ( height - 100 / this->getLength().number() );
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

bool GWSGraphEdge::equals( const GWSGraphEdge *other) const{
    return this->getFromNode()->equals( other->getFromNode() ) && this->getToNode()->equals( other->getToNode() );
}

bool GWSGraphEdge::equalsReversed(const GWSGraphEdge *other) const{
    return this->getFromNode()->equals( other->getToNode() ) && this->getToNode()->equals( other->getFromNode() );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGraphEdge::setLength(GWSLengthUnit length){
    this->length = length;
}
