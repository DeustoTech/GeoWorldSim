#ifndef GWSEDGEVISITOR_H
#define GWSEDGEVISITOR_H

#include <QObject>
#include <QDebug>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>
#include <lemon/maps.h>

#include <QMap>

#include "../../util/graph/Edge.h"

class GWSEdgeVisitor : public lemon::ListDigraph::ArcMap<double>
{

public:

    GWSEdgeVisitor( lemon::ListDigraph* routing_graph );

    // GETTERS
    QMap< lemon::ListDigraph::Arc , double > arc_costs;

    // SETTERS

    // OPERATORS
    double operator[](Key arc) const;

};

#endif // GWSEDGEVISITOR_H
