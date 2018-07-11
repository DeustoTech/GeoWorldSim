#include "GraphEdgeArcMap.h"

#include "utils/routing/Routing.h"

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

    GWSGraphEdge* edge = this->parent->getEdgeFromArc( arc );
    if( !edge ){ return INFINITY; }

    // Get accumulated cost (generally arrival time) at from_node of edge
    GWSGraphNode* from = edge->getFromNode();
    double acc_cost = this->acc_costs.value( from , 0 );
    double cost_for_edge = edge->getCost( acc_cost ) + from->getCost();

    // Set accumulated cost at to_edge of node
    GWSGraphNode* to = edge->getToNode();
    this->acc_costs.insert( to , qMin( cost_for_edge , this->acc_costs.value( to , INFINITY ) ) );

    return cost_for_edge;
}
