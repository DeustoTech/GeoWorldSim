#include "MoveAlongStagedRouteSkill.h"

#include "../../environment/Environment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
//#include "../../behaviour/move/FindBestRouteBehaviour.h"
#include "../../util/random/UniformDistribution.h"

#include "../../util/routing/TSPRouting.h"

QString MoveAlongStagedRouteSkill::EDGES_CLASS_PROP = "edges_class";

MoveAlongStagedRouteSkill::MoveAlongStagedRouteSkill() : MoveThroughRouteSkill(){
    this->setProperty( EDGES_CLASS_PROP , "GWSAgent" );
}


/**********************************************************************
 GETTERS
**********************************************************************/

// Generate graph, give me all the agents in the network environment, and generate the graph with them


/*void MoveAlongStagedRouteSkill::generateTSPGraph(){
    // Generate graph of all GWSAgents in the network environment
    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( this->getProperty( MoveThroughAllContainersSkill::EDGES_CLASS_PROP ).toString() );

    // Generate TSP algorithm with those edges:
    this->tsp_route = new GSSTSPRouting( graph->getEdges() );
}*/

/**********************************************************************
 METHODS
**********************************************************************/

void MoveAlongStagedRouteSkill::step(  GWSTimeUnit movement_duration ){

    // Extract current coordinates of Skilled GWSAgent
    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );

    if( !agent_geom ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getId() );
        return;
    }
    GWSCoordinate current_coor = agent_geom->getCentroid();
    GWSCoordinate agent_home_coor = GWSCoordinate( agent->getProperty( "home_coordX").toDouble() , agent->getProperty( "home_coordY").toDouble() );
    QSharedPointer<MoveThroughRouteSkill> move_throughroute = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();


    //QList< GWSCoordinate> route_stages_coors = this->property( ROUTE_STAGES_PROP );
    //QSharedPointer<FindBestRouteBehaviour> ordered_route = agent->getBehaviour( FindBestRouteBehaviour::staticMetaObject.className() ).dynamicCast<FindBestRouteBehaviour>();
    //QList< GWSCoordinate> ordered_route_coors = ordered_route->route_stages_coordinates;


    // Generate random vector to try TSP algorithms
    double lat_max = 43.28539;
    double lat_min = 43.27554;
    double lon_max = -2.85024;
    double lon_min = -2.87092;

    int n = 10;
    QList<GWSCoordinate> container_route_nodes;
    for(int i = 0; i < n; i++){
        GWSCoordinate coor = GWSCoordinate( ( lon_max - lon_min ) * UniformDistribution::uniformDistribution()  + lon_min , ( lat_max - lat_min ) * UniformDistribution::uniformDistribution() + lat_min );
        container_route_nodes.append( coor );
    }

    //const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( GWSAgent::staticMetaObject.className()  );
    // Generate TSP algorithm with those edges:

    //QList< QSharedPointer<GWSGraphEdge> > edges = graph->getEdges();

    // Generate TSP algorithm with those edges:
    //GSSTSPRouting* container_route = new GSSTSPRouting( edges );

    // Get nearest neighbour given start coordinates and containers to visit:
    //QList< GWSCoordinate > container_route_stages = container_route->nearestNeighborTsp( agent_home_coor , container_route_nodes , agent_home_coor );

    // Order the nodes to get best route. This is the route to follow.
    //QList< GWSCoordinate > container_route_stages_ordered = container_route->orderCircularTsp( agent_home_coor , agent_home_coor , container_route_stages);

    foreach( GWSCoordinate stage_coor , container_route_nodes ){


            qWarning() << QString("Should be moving towards stage %1 ").arg( stage_coor.toString() );

            GWSCoordinate current_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
            qDebug() <<  QString("Position = %1 ").arg( current_position.toString() );

            if( current_position == stage_coor ){

                qWarning() <<  QString("Position = %1 ").arg( current_position.toString() );
                qWarning() << QString("Arrived at stage %1 ").arg( stage_coor.toString() );

                return;
            }


            move_throughroute->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , stage_coor.getX() );
            move_throughroute->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , stage_coor.getY() );

            // Calculate speed
            GWSLengthUnit distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid().getDistance( stage_coor );
            if( move_throughroute->getCurrentSpeed() == 0.0 ){
                move_throughroute->changeSpeed( 1 );
            }

            while ( move_throughroute->getRouteDestination() != current_position ){

                move_throughroute->move( movement_duration );

            }


            qWarning() << QString("Arrived at stage %1 ").arg( stage_coor.toString() );
            GWSCoordinate final_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
            qDebug() <<  QString("Position = %1 ").arg( final_position.toString() );

    }



}
