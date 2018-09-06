#include "GraphEdgeArcMap.h"

#include "../../util/routing/Routing.h"

GWSGraphEdgeArcMap::GWSGraphEdgeArcMap(GWSRouting *parent) : ListDigraph::ArcMap<double>( *parent->routing_graph ){
    this->parent = parent;
}

/**********************************************************************
 GETTERS
**********************************************************************/

/**********************************************************************
 SETTERS
**********************************************************************/

/**********************************************************************
 OPERATORS
**********************************************************************/

double GWSGraphEdgeArcMap::operator [](Key arc) const{

    QSharedPointer<GWSGraphEdge> edge = this->parent->getEdgeFromArc( arc );
    if( !edge ){ return Q_INFINITY; }

    // Get accumulated cost (generally arrival time) at from_node of edge
    QSharedPointer<GWSGraphNode> from = edge->getFromNode();
    double acc_cost = this->accumulated_minimum_cost_at_nodes.value( from , 0 );
    double cost_for_edge = edge->getCost( acc_cost ) + from->getCost();

    // Set accumulated cost at to_edge of node
    QSharedPointer<GWSGraphNode> to = edge->getToNode();
    this->accumulated_minimum_cost_at_nodes.insert( to , qMin( cost_for_edge , this->accumulated_minimum_cost_at_nodes.value( to , Q_INFINITY ) ) );

    return cost_for_edge;
}
