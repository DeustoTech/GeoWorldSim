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

template <class T = GWSEdge>
class GWSEdgeVisitor : public lemon::ListDigraph::ArcMap<double>
{

public:

    GWSEdgeVisitor( lemon::ListDigraph* routing_graph ) : lemon::ListDigraph::ArcMap<double>( *routing_graph ){}

    // OPERATORS
    double operator[](Key arc) const{
        return this->edges.value( arc ).cost;
    }

    // STORAGE
    QMap< lemon::ListDigraph::Arc , T > edges;

};

#endif // GWSEDGEVISITOR_H
