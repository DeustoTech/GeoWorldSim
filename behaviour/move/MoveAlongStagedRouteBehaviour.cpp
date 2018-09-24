#include "MoveAlongStagedRouteBehaviour.h"

#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
//#include "../../behaviour/move/FindBestRouteBehaviour.h"
#include "../../app/App.h"
#include "../../skill/move/MoveAlongStagedRouteSkill.h"
#include "../../util/random/UniformDistribution.h"



#include "../../environment/network_environment/NetworkEnvironment.h"

MoveAlongStagedRouteBehaviour::MoveAlongStagedRouteBehaviour() : GWSBehaviour()
{

}



/**********************************************************************
 GETTERS
**********************************************************************/

bool MoveAlongStagedRouteBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QSharedPointer<MoveAlongStagedRouteSkill> mv = agent->getSkill( MoveAlongStagedRouteSkill::staticMetaObject.className() ).dynamicCast<MoveAlongStagedRouteSkill>();
    // No move skill
    if( !mv ){
        qWarning() << QString("Agent %1 %2 wants to move but has no MoveAlongStagedRouteSkill").arg( agent->staticMetaObject.className() ).arg( agent->getId() );
        return false;
    }
    // No destination for MoveThroughRouteSkill
    //QSharedPointer<FindBestRouteBehaviour> ordered_route = agent->getBehaviour( FindBestRouteBehaviour::staticMetaObject.className() ).dynamicCast<FindBestRouteBehaviour>();
    //if( ordered_route->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP ).isNull() || mv->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP ).isNull() ){
      //  return false;
    //}

    GWSCoordinate home_position = GWSCoordinate( agent->getProperty( "home_coordX").toDouble() , agent->getProperty( "home_coordY").toDouble() );
    GWSCoordinate current_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    //if ( current_position == home_position ){
        // Return true because we have finished the route and gone back to the depot.
        // CAUTION! Need to check whether the best route passes through the depot though!
      //  return true;
    //}

    qWarning() << "Should go into the behave() method.";
    GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
    env->getId();

    return false;
}

/**********************************************************************
 METHODS
**********************************************************************/

bool MoveAlongStagedRouteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_home_coor = GWSCoordinate( agent->getProperty( "home_coordX").toDouble() , agent->getProperty( "home_coordY").toDouble() );


    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent can move
    QSharedPointer<MoveAlongStagedRouteSkill> move_alongstagedroute_skill = agent->getSkill( MoveAlongStagedRouteSkill::staticMetaObject.className() ).dynamicCast<MoveAlongStagedRouteSkill>();
    if( !move_alongstagedroute_skill ){
        qWarning() << QString("Agent %1 does not have a move_throughroute_skill").arg( agent->getId() );
        return false;
    }

    /*GWSCoordinate destination_coor = move_throughcontainers_skill->getRouteDestination();
    if( !destination_coor.isValid() ){
        return true;
    }*/

    // Calculate speed
    //GWSLengthUnit distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid().getDistance( destination_coor );
    //if( move_alongstagedroute_skill->getCurrentSpeed() == 0.0 ){
      //  move_alongstagedroute_skill->changeSpeed( 1 );
    //}

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Generate random vector to try TSP algorithms
    double lat_max = 43.28539;
    double lat_min = 43.27554;
    double lon_max = -2.85024;
    double lon_min = -2.87092;

    int n = 500;
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
   // QList< GWSCoordinate > container_route_stages = container_route->nearestNeighborTsp( agent_home_coor , container_route_nodes , agent_home_coor );

    // Order the nodes to get best route. This is the route to follow.
    //QList< GWSCoordinate > container_route_stages_ordered = container_route->orderCircularTsp( agent_home_coor , agent_home_coor , container_route_stages);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++

    QSharedPointer<MoveThroughRouteSkill> move_throughroute = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();


    foreach( GWSCoordinate stage_coor , container_route_nodes ){



            qWarning() << QString("Should be moving towards stage %1 ").arg( stage_coor.toString() );

            GWSCoordinate current_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
            qDebug() <<  QString("Position = %1 ").arg( current_position.toString() );


            move_throughroute->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , stage_coor.getX() );
            move_throughroute->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , stage_coor.getY() );

            GWSCoordinate destination_coor = move_throughroute->getRouteDestination();
            if( !destination_coor.isValid() ){
                return true;
            }

            // Calculate speed
            GWSLengthUnit distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid().getDistance( destination_coor );
            if( move_throughroute->getCurrentSpeed() == 0.0 ){
                move_throughroute->changeSpeed( 1 );
            }

            if( current_position == destination_coor ){

                qWarning() <<  QString("Position = %1 ").arg( current_position.toString() );
                qWarning() << QString("Arrived at stage %1 ").arg( destination_coor.toString() );

                return true;
            }


            // Move towards
            while ( current_position != destination_coor ){
                GWSCoordinate zero_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
                move_throughroute->move( duration_of_movement );
                GWSCoordinate after_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
                qDebug() << after_position.toString();
                current_position = after_position;
            }

            GWSCoordinate after_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

            emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );


    }

    qDebug() << "Route completed! Should go back home.";

    move_throughroute->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , agent_home_coor.getX() );
    move_throughroute->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , agent_home_coor.getY() );

    GWSCoordinate  current_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
    GWSCoordinate destination_coor = move_throughroute->getRouteDestination();

    while ( current_position != destination_coor ){
        GWSCoordinate zero_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
        move_throughroute->move( duration_of_movement );
        GWSCoordinate after_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
        qDebug() << after_position.toString();
        current_position = after_position;
    }

    GWSCoordinate after_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );

    return true;
}



