#include "MoveThroughRouteBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

QString MoveThroughRouteBehaviour::SET_NETWORK_TYPE = "network_type";
QString MoveThroughRouteBehaviour::SET_DESTINATION_X_VALUE = "destination_x_value";
QString MoveThroughRouteBehaviour::SET_DESTINATION_Y_VALUE = "destination_y_value";
QString MoveThroughRouteBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString MoveThroughRouteBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";


MoveThroughRouteBehaviour::MoveThroughRouteBehaviour() : GWSBehaviour(){
}

MoveThroughRouteBehaviour::~MoveThroughRouteBehaviour(){
}


/**********************************************************************
 INITIALIZE
**********************************************************************/

void MoveThroughRouteBehaviour::initialize(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<MoveThroughRouteSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    if( movethroughroute_skill.isNull() ){
        movethroughroute_skill = QSharedPointer<MoveThroughRouteSkill>( new MoveThroughRouteSkill() );
        agent->addSkill( movethroughroute_skill );
    }

}

/**********************************************************************
 METHODS
**********************************************************************/

QJsonArray MoveThroughRouteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    QSharedPointer<MoveThroughRouteSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();

    QJsonValue x_destination = this->getProperty( SET_DESTINATION_X_VALUE );
    QJsonValue y_destination = this->getProperty( SET_DESTINATION_Y_VALUE );
    agent->setProperty( MoveThroughRouteSkill::AGENT_ROUTE_DESTINATION_X_PROP , x_destination );
    agent->setProperty( MoveThroughRouteSkill::AGENT_ROUTE_DESTINATION_Y_PROP , y_destination );

    GWSCoordinate destination_coor = movethroughroute_skill->getRouteDestination();
    if( !destination_coor.isValid() ){
        return this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray();
    }

    // Move towards
    agent->setProperty( MoveThroughRouteSkill::AGENT_MOVE_NETWORK_TYPE_PROP , this->getProperty( SET_NETWORK_TYPE ) );
    movethroughroute_skill->move( duration_of_movement );

    QSharedPointer<GWSGeometry> agent_geom_post = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    GWSCoordinate agent_position_post = agent_geom_post->getCentroid();

    // Set NEXTS behaviour
    if ( agent_position_post.getDistance( destination_coor ) < GWSLengthUnit(1) ){
        return this->getProperty( NEXTS_IF_ARRIVED ).toArray();
    }

    if ( agent_position_post != destination_coor ){
        return this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray();
    }

}
