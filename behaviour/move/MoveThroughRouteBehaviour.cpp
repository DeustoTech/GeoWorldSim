#include "MoveThroughRouteBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveThroughRouteSkill.h"


QString MoveThroughRouteBehaviour::BEHAVIOUR_DESTINATION_X_VALUE = "x_value";
QString MoveThroughRouteBehaviour::BEHAVIOUR_DESTINATION_Y_VALUE = "y_value";
QString MoveThroughRouteBehaviour::BEHAVIOUR_NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString MoveThroughRouteBehaviour::BEHAVIOUR_NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";


MoveThroughRouteBehaviour::MoveThroughRouteBehaviour() : GWSBehaviour(){
}

MoveThroughRouteBehaviour::~MoveThroughRouteBehaviour(){
}


/**********************************************************************
 METHODS
**********************************************************************/

QStringList MoveThroughRouteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<MoveThroughRouteSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    if( movethroughroute_skill.isNull() ){
        movethroughroute_skill = QSharedPointer<MoveThroughRouteSkill>( new MoveThroughRouteSkill() );
        agent->addSkill( movethroughroute_skill );
    }

    QVariant x_destination = this->getProperty( BEHAVIOUR_DESTINATION_X_VALUE );
    QVariant y_destination = this->getProperty( BEHAVIOUR_DESTINATION_Y_VALUE );
    movethroughroute_skill->setProperty( MoveThroughRouteSkill::SKILL_ROUTE_DESTINATION_X_PROP , x_destination );
    movethroughroute_skill->setProperty( MoveThroughRouteSkill::SKILL_ROUTE_DESTINATION_Y_PROP , y_destination );

    GWSCoordinate destination_coor = movethroughroute_skill->getRouteDestination();
    if( !destination_coor.isValid() ){
        return this->getNexts( BEHAVIOUR_NEXTS_IF_NOT_ARRIVED );
    }

    QSharedPointer<GWSGeometry> agent_geom_init = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    GWSCoordinate agent_position_init = agent_geom_init->getCentroid();

    // Move towards
    movethroughroute_skill->move( duration_of_movement );

    QSharedPointer<GWSGeometry> agent_geom_post = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    GWSCoordinate agent_position_post = agent_geom_post->getCentroid();

    // Set NEXTS behaviour
    if ( agent_position_post == destination_coor ){
        return this->getNexts( BEHAVIOUR_NEXTS_IF_ARRIVED );
    }

    if ( agent_position_post != destination_coor ){
        return this->getNexts( BEHAVIOUR_NEXTS_IF_ARRIVED );
    }

}
