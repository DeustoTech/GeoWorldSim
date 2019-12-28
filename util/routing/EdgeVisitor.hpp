#ifndef EDGEVISITOR_H
#define EDGEVISITOR_H

#include <QObject>
#include <QDebug>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>
#include <lemon/maps.h>

#include <QMap>
#include "../../util/graph/Edge.h"

namespace geoworldsim {
namespace graph {


class EdgeVisitor : public lemon::ListDigraph::ArcMap<double>
{

public:

    EdgeVisitor( lemon::ListDigraph* routing_graph ) : lemon::ListDigraph::ArcMap<double>( *routing_graph ){}

    // OPERATORS
    double operator[](Key arc) const{
        return this->edges.value( arc ).cost;
    }

    // STORAGE
    QMap< lemon::ListDigraph::Arc , Edge > edges;

};

}
}

#endif // EDGEVISITOR_H
