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

    qDebug() << 2;

    QSharedPointer<GWSGraphEdge> edge = this->parent->arc_to_edges.value( arc );
    if( !edge ){ return Q_INFINITY; }

    // Get accumulated cost (generally arrival time) at from_node of edge
    GWSCoordinate from = edge->getFrom();
    double acc_cost = this->accumulated_minimum_cost.value( from , 0 );
    double cost_for_edge = edge->getCost( acc_cost );

    // Set accumulated cost at to_edge of node
    GWSCoordinate to = edge->getTo();
    this->accumulated_minimum_cost.insert( to , qMin( cost_for_edge , this->accumulated_minimum_cost.value( to , Q_INFINITY ) ) );

    return cost_for_edge;
}
