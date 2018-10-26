#ifndef GWSGRAPHEDGEVISITOR_H
#define GWSGRAPHEDGEVISITOR_H

#include <QObject>
#include <QDebug>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>
#include <lemon/maps.h>

#include "../../util/graph/GraphEdge.h"

QT_FORWARD_DECLARE_CLASS(GWSRouting)

class GWSGraphEdgeVisitor : public lemon::ListDigraph::ArcMap<double>
{

public:

    GWSGraphEdgeVisitor( lemon::ListDigraph* routing_graph , GWSRouting* parent );

    // GETTERS

    // SETTERS

    // OPERATORS
    double operator[](Key arc) const;

private:
    GWSRouting* parent;
    mutable QMap< GWSCoordinate , double > accumulated_minimum_cost; // Accumulated minimum costs at reached coordinates

};

#endif // GWSGRAPHEDGEVISITOR_H
