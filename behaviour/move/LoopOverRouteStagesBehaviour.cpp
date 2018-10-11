#include "LoopOverRouteStagesBehaviour.h"

LoopOverRouteStagesBehaviour::LoopOverRouteStagesBehaviour() : GWSBehaviour(){
}

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../util/random/UniformDistribution.h"
#include "../../util/routing/TSPRouting.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "ContainerAgent.h"

#include "../../object/ObjectFactory.h"

#include <QJsonDocument>


/**********************************************************************
 GETTERS
**********************************************************************/


bool LoopOverRouteStagesBehaviour::canContinueToNext(){

    QSharedPointer< GWSAgent> agent = this->getAgent();

    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();

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


// Generate Ordered TSP route for ContainerAgents

void LoopOverRouteStagesBehaviour::generateOrderedTSPRoute() {

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_home_coor = GWSCoordinate( agent->getProperty( "home_coordX" ).toDouble(), agent->getProperty( "home_coordY" ).toDouble() );

    // Generate list of containers to visit:
    QMap<QString , GWSCoordinate > container_coord_id_array;
    QList < GWSCoordinate > container_coord_array;

    QList<QSharedPointer<GWSAgent> > agents = GWSAgentEnvironment::globalInstance()->getByClass( ContainerAgent::staticMetaObject.className()) ;
    foreach ( QSharedPointer<GWSAgent> a, agents  ){

         GWSCoordinate container_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( a )->getCentroid();
         QString container_id = a->getProperty("@id").toString();
         container_coord_array.append( container_coord ) ;
         container_coord_id_array.insert( container_id , container_coord );
     }

    // Generate graph:
    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( GWSAgent::staticMetaObject.className()  );

    // Get graph edges:
    QList< QSharedPointer< GWSGraphEdge > > edges = graph->getEdges();

    // Generate TSP algorithm with those edges:
    GWSTSPRouting* container_route = new GWSTSPRouting( edges );

    // Get nearest neighbour given start coordinates and containers to visit:
    QList< GWSCoordinate > container_tsp_route_coord_array = container_route->nearestNeighborTsp( agent_home_coor , container_coord_array , agent_home_coor );
    foreach (GWSCoordinate c, container_tsp_route_coord_array) {
    }

    // Order the nodes to get best route. This is the route to follow.
    QList< GWSCoordinate > ordered_container_tsp_route_coord_array = container_route->orderCircularTsp( agent_home_coor , agent_home_coor , container_tsp_route_coord_array );
    foreach (GWSCoordinate c, ordered_container_tsp_route_coord_array) {
    }

    // Compare the coordinates of the ordered route and those in the initial QPair QList of coors and IDs to extract
    // the ID of the ordered coors and generate a new QList of QPairs:
    foreach (GWSCoordinate c, ordered_container_tsp_route_coord_array) {
        this->ordered_container_tsp_route_coord_id_array.append( qMakePair( c , container_coord_id_array.key( c ) ) );
    }

}

/**********************************************************************
 METHODS
**********************************************************************/


bool LoopOverRouteStagesBehaviour::behave(){

     if ( this->ordered_container_tsp_route_coord_id_array.isEmpty() ){
         this->generateOrderedTSPRoute();
     }

     QSharedPointer<GWSAgent> agent = this->getAgent();
     QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();

     int loop_stage = agent->getProperty( "loop_stage" ).toInt();
     int route_size = this->ordered_container_tsp_route_coord_id_array.size();
     //qDebug() << route_size;

     if ( loop_stage <= ( route_size - 1 )  ){

        QPair<GWSCoordinate, QString> pair = this->ordered_container_tsp_route_coord_id_array.at( loop_stage );
        GWSCoordinate stage_coors = pair.first;
        QString stage_id = pair.second;

        double x = stage_coors.getX();
        double y = stage_coors.getY();

        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , x );
        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , y );

        agent->setProperty( "compare_agent_id" , stage_id );

        loop_stage = loop_stage + 1;
        }
       else {

        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , agent->getProperty( "home_coordX" ).toDouble() );
        mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , agent->getProperty( "home_coordY" ).toDouble() );

        loop_stage = 0;


    }


    agent->setProperty( "loop_stage" , loop_stage );

    return true;

}
