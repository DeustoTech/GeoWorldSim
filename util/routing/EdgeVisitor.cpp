#include "EdgeVisitor.h"

#include "../../util/routing/Routing.hpp"

GWSEdgeVisitor::GWSEdgeVisitor( lemon::ListDigraph* routing_graph ) : lemon::ListDigraph::ArcMap<double>( *routing_graph ){
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

double GWSEdgeVisitor::operator [](Key arc) const {
    return this->arc_costs.value( arc );
}
