#ifndef GWSGRAPHEDGEVISITOR_H
#define GWSGRAPHEDGEVISITOR_H

#include <QObject>
#include <QDebug>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>
#include <lemon/maps.h>

#include <QMap>

#include "../../util/graph/GraphEdge.h"


QT_FORWARD_DECLARE_CLASS(GWSRouting)

class GWSGraphEdgeVisitor : public lemon::ListDigraph::ArcMap<double>
{

public:

    GWSGraphEdgeVisitor( lemon::ListDigraph* routing_graph );

    // GETTERS
    QMap< lemon::ListDigraph::Arc , double > arc_costs;

    // SETTERS

    // OPERATORS
    double operator[](Key arc) const;

};

#endif // GWSGRAPHEDGEVISITOR_H
