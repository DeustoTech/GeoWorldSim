#ifndef GRAPHSERVICEAREACALCULATOR_H
#define GRAPHSERVICEAREACALCULATOR_H

#include "utils/service_area/ServiceAreaCalculator.h"
#include "utils/graph/Graph.h"

#include "utils/geometry/GeometryFactory.h"
#include "geos/geom/Geometry.h"

using namespace geos::geom;

class GraphServiceAreaCalculator : public ServiceAreaCalculator
{
    Q_OBJECT

public:
    GraphServiceAreaCalculator( GWSObject* parent = 0 );

    // METHODS
    QList<const GWSGraphEdge*> getReachableEdges( const GWSGraph* graph , const GWSGraphNode* from , double cost );

private:
    void getReachableEdgesRecursive( const GWSGraph* graph , const GWSGraphNode* from , double cost , QList<const GWSGraphNode*>* visited_nodes , QList<const GWSGraphEdge*>* reachable_edges );
};

#endif // GRAPHSERVICEAREACALCULATOR_H
