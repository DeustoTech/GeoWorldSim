#include "LoopOverRouteStagesBehaviour.h"

LoopOverRouteStagesBehaviour::LoopOverRouteStagesBehaviour() : GWSBehaviour(){
}

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../util/random/UniformDistribution.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "ContainerAgent.h"

#include "../../object/ObjectFactory.h"

#include <QJsonDocument>


/**********************************************************************
 GETTERS
**********************************************************************/


bool LoopOverRouteStagesBehaviour::continueToNext(){

    QSharedPointer< GWSAgent> agent = this->getAgent();

    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();

    qDebug() << mv->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP );
    qDebug() << mv->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP );

    QString x = mv->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP ).toString();
    QString y = mv->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP ).toString();

    // No destination for MoveThroughRouteSkill
    if( x.isNull() || y.isNull() ){
        return false;
    }

    GWSCoordinate current_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    if ( current_position == mv->getRouteDestination() ){
        return false;
    }
    return true;

}



QList<GWSCoordinate> LoopOverRouteStagesBehaviour::generateRouteCoordinateArray( ) const{

    // SET ROUTE STAGES
    // Generate random vector to try TSP algorithms
    double lat_max = 43.28539;
    double lat_min = 43.27554;
    double lon_max = -2.85024;
    double lon_min = -2.87092;

    int n = 3;
    QList<GWSCoordinate> route_nodes;
    for(int i = 0; i < n; i++){
        GWSCoordinate coor = GWSCoordinate( ( lon_max - lon_min ) * UniformDistribution::uniformDistribution()  + lon_min , ( lat_max - lat_min ) * UniformDistribution::uniformDistribution() + lat_min );
        route_nodes.append( coor );
    }
    return route_nodes;
}

/**********************************************************************
 METHODS
**********************************************************************/


bool LoopOverRouteStagesBehaviour::behave(){


     if( route_nodes.isEmpty() ){
         route_nodes = this->generateRouteCoordinateArray( );
     }


    // APPLY TSP ALGORITHM TO OBTAIN OPTIMAL STAGE ORDER
    //const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( GWSAgent::staticMetaObject.className()  );
    // Generate TSP algorithm with those edges:

    //QList< QSharedPointer<GWSGraphEdge> > edges = graph->getEdges();

    // Generate TSP algorithm with those edges:
    //GSSTSPRouting* container_route = new GSSTSPRouting( edges );

    /* Generate list of containers to visit:
    QList < QPair <GWSCoordinate , QString > container_array;
    QList < GWSCoordinate  > container_coord_array;

    foreach ( QSharedPointer<GWSAgent> a, GWSAgentEnvironment::globalInstance()->getByClass( ContainerAgent::staticMetaObject.className())  ){
          GWSCoordinate container_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( a )->getCentroid();
          QString container_id = a->getId();
          container_array.append(qMakePair( container_coord, container_id ) );
          container_coord_array.append( container_coord );
      }



    // Get nearest neighbour given start coordinates and containers to visit:
    //QList< GWSCoordinate > container_route_stages = container_route->nearestNeighborTsp( agent_home_coor , container_coord_array.first , agent_home_coor );

    // Order the nodes to get best route. This is the route to follow.
    //QList< GWSCoordinate > container_route_stages_ordered = container_route->orderCircularTsp( agent_home_coor , agent_home_coor , container_route_stages);
    */

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();

    int loop_stage = agent->getProperty( "loop_stage" ).toInt();

    /* if ( loop_stage < container_array.size() ){
     *
                QPair<QString, QString> pair = container_coord_array.at( loop_stage );
                QList <GWSCoordinate> stage_coors = pair.first;

                double x = stage_coors.getX();
                double y = stage_coors.getY();

                mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , x );
                mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , y );

                agent->setProperty( "compare_agent_id" , pair.second );
                loop_stage = loop_stage + 1;
     * }
     *
        if ( loop_stage >= container_array.size() ){
        qWarning() << "Finished your route, set home as destination!";
        //GWSCoordinate agent_home_coor = GWSCoordinate( agent->getProperty( "home_coordX").toDouble() , agent->getProperty( "home_coordY").toDouble() );

        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , agent->getProperty( "home_coordX").toDouble() );
        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , agent->getProperty( "home_coordY").toDouble() );

        loop_stage = 0;
       }
    */
    if ( loop_stage >= 3 ){

        qWarning() << "Finished your route, set home as destination!";
        //GWSCoordinate agent_home_coor = GWSCoordinate( agent->getProperty( "home_coordX").toDouble() , agent->getProperty( "home_coordY").toDouble() );

        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , agent->getProperty( "home_coordX").toDouble() );
        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , agent->getProperty( "home_coordY").toDouble() );
       // return;
        loop_stage = 0;
    }

    else{

        GWSCoordinate stage_coors =  route_nodes[ loop_stage ] ; //container_route_stages_ordered[ loop_stage ]
        double x = stage_coors.getX();
        double y = stage_coors.getY();

        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , x );
        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , y );

        agent->setProperty( "compare_agent_id" , loop_stage );
        loop_stage = loop_stage + 1;

        // We should be extracting the the IDs of the container located at this position so that we can use it as a
        // comparison parameter afterwards

       // return;
    }

    agent->setProperty( "loop_stage" , loop_stage);

    return true;

}
