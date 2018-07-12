#ifndef SOCIALENVIRONMENT_H
#define SOCIALENVIRONMENT_H

#include "../../environment/Environment.h"
#include "../../util/graph/Graph.h"
#include "../../util/graph/GraphNode.h"
#include "../../util/graph/GraphEdge.h"

using namespace geos::geom;
using namespace geos::planargraph;

class GWSSocialEnvironment : public GWSEnvironment
{
    Q_OBJECT
    friend class GWSEnvironment; // Environment can access

public:
    static GWSSocialEnvironment* globalInstance();

private:
    GWSSocialEnvironment();
    GWSSocialEnvironment(GWSSocialEnvironment const&);
    ~GWSSocialEnvironment();

    QMap<QString, GWSGraph*> spatial_graph; // Social graphs

};

#endif // SOCIALENVIRONMENT_H
