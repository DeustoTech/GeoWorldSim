#ifndef GSSGRAPHEDGEARCMAP_H
#define GSSGRAPHEDGEARCMAP_H

#include <QObject>
#include <QDebug>

#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>
#include <lemon/maps.h>

#include "util/graph/GraphEdge.h"

QT_FORWARD_DECLARE_CLASS(GWSRouting)

using namespace lemon;

class GWSGraphEdgeArcMap : public ListDigraph::ArcMap<double>
{

public:

    GWSGraphEdgeArcMap( GWSRouting* parent );

    // GETTERS

    // SETTERS

    // OPERATORS
    double operator[](Key arc) const;

private:
    GWSRouting* parent;
    mutable QMap< GWSGraphNode* , double > acc_costs; // Accumulated minimum costs at reached nodes

};

#endif // GSSGRAPHEDGEARCMAP_H
