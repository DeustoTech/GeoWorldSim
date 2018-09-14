#include "MoveTSPBehaviour.h"

#include "../../skill/move/MoveTSPSkill.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../app/App.h"

MoveTSPBehaviour::MoveTSPBehaviour() : GWSBehaviour()
{
}

MoveTSPBehaviour::~MoveTSPBehaviour(){
}



/**********************************************************************
 GETTERS
**********************************************************************/

bool MoveTSPBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<MoveTSPSkill> mv = agent->getSkill( MoveTSPSkill::staticMetaObject.className() ).dynamicCast<MoveTSPSkill>();

    // No moveThroughRoute skill
    if( !mv ){
        qWarning() << QString("Agent %1 %2 wants to move but has no MoveSkill").arg( agent->staticMetaObject.className() ).arg( agent->getId() );
        return false;
    }

    // No destination for MoveTSPSkill
    if( mv->getProperty( MoveTSPSkill::ROUTE_DESTINATION_X_PROP ).isNull() || mv->getProperty( MoveTSPSkill::ROUTE_DESTINATION_Y_PROP ).isNull() ){
        return false;
    }

    GWSCoordinate current_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    if ( current_position == mv->getRouteDestination() ){
        // Return true because we have really reached
        return true;
    }

    return false;
}




/**********************************************************************
 METHODS
**********************************************************************/

bool MoveTSPBehaviour::behave(){

    qDebug() << "I am running!";

    QSharedPointer<GWSAgent> agent = this->getAgent();


    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent can move
    QSharedPointer<MoveTSPSkill> move_TSP_skill = agent->getSkill( MoveTSPSkill::staticMetaObject.className() ).dynamicCast<MoveTSPSkill>();

    if( !move_TSP_skill ){
        qWarning() << QString("Agent %1 does not have a move_throughroute_skill").arg( agent->getId() );
        return false;
    }

    GWSCoordinate agent_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( this->getProperty( MoveTSPSkill::EDGES_CLASS_PROP ).toString() );
    QList< QSharedPointer<GWSGraphEdge> > edges = graph->getEdges();

    foreach ( QSharedPointer<GWSGraphEdge> e, edges ){
        GWSCoordinate edge_coors = GWSCoordinate( e->getTo().getX(), e->getTo().getY() );
        //qDebug() << edge_coors;
    }

    if( this->route.isEmpty() ){
        // Generate pending route
        this->route = this->routing_graph->dijkstraShortestPath( current_coor , destination_coor);
    }

    move_TSP_skill->move( duration_of_movement );
    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );




}
