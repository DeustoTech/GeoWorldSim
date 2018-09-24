#include "FindBestRouteBehaviour.h"

FindBestRouteBehaviour::FindBestRouteBehaviour() : GWSBehaviour(){
}


/**********************************************************************
 GETTERS
**********************************************************************/


bool FindBestRouteBehaviour::continueToNext(){

}

/**********************************************************************
 METHODS
**********************************************************************/


bool FindBestRouteBehaviour::behave(){

    // Generate random vector to try TSP algorithms
    double lat_max = 43.28539;
    double lat_min = 43.27554;
    double lon_max = -2.85024;
    double lon_min = -2.87092;

    /*int n = 10;
    QList<GWSCoordinate> container_route_nodes;
    for(int i = 0; i < n; i++){
        GWSCoordinate coor = GWSCoordinate( ( lon_max - lon_min ) * UniformDistribution::uniformDistribution()  + lon_min , ( lat_max - lat_min ) * UniformDistribution::uniformDistribution() + lat_min );
        container_route_nodes.append( coor );
    }*/

    //const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( GWSAgent::staticMetaObject.className()  );
    // Generate TSP algorithm with those edges:

    //QList< QSharedPointer<GWSGraphEdge> > edges = graph->getEdges();

    // Generate TSP algorithm with those edges:
    //GSSTSPRouting* container_route = new GSSTSPRouting( edges );

    // Get nearest neighbour given start coordinates and containers to visit:
    //QList< GWSCoordinate > container_route_stages = container_route->nearestNeighborTsp( agent_home_coor , container_route_nodes , agent_home_coor );

    // Order the nodes to get best route. This is the route to follow.
    //QList< GWSCoordinate > container_route_stages_ordered = container_route->orderCircularTsp( agent_home_coor , agent_home_coor , container_route_stages);

}
