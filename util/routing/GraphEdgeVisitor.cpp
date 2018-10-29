#include "GraphEdgeVisitor.h"

#include "../../util/routing/Routing.h"

GWSGraphEdgeVisitor::GWSGraphEdgeVisitor( lemon::ListDigraph* routing_graph ) : lemon::ListDigraph::ArcMap<double>( *routing_graph ){
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

double GWSGraphEdgeVisitor::operator [](Key arc) const {
    return this->arc_costs.value( arc );
}
