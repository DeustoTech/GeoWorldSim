#include "MoveThroughRouteBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

QString MoveThroughRouteBehaviour::AGENT_ROUTE_NETWORK_TYPE = "transport_network_type";
QString MoveThroughRouteBehaviour::AGENT_ROUTE_DESTINATION_X_VALUE = "route_destination_x_value";
QString MoveThroughRouteBehaviour::AGENT_ROUTE_DESTINATION_Y_VALUE = "route_destination_y_value";
QString MoveThroughRouteBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString MoveThroughRouteBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";


MoveThroughRouteBehaviour::MoveThroughRouteBehaviour() : MoveBehaviour(){
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

    QJsonValue x_destination = this->getProperty( AGENT_ROUTE_DESTINATION_X_VALUE );
    QJsonValue y_destination = this->getProperty( AGENT_ROUTE_DESTINATION_Y_VALUE );
    GWSCoordinate destination_coor = GWSCoordinate( x_destination.toDouble() , y_destination.toDouble() );
    if( !destination_coor.isValid() ){
        return this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray();
    }

    // Calculate speed
    GWSSpeedUnit current_speed = GWSSpeedUnit( this->getProperty( AGENT_CURRENT_SPEED ).toDouble() );
    GWSSpeedUnit max_speed = GWSSpeedUnit( this->getProperty( AGENT_MAX_SPEED ).toDouble() );

    if( current_speed == 0 ){
        current_speed = movethroughroute_skill->calculateNewSpeed( current_speed , max_speed , 1.4 );
        this->setProperty( AGENT_CURRENT_SPEED , current_speed.number() );
    }

    // Move towards
    QString graph_type = this->getProperty( AGENT_ROUTE_NETWORK_TYPE ).toString();
    movethroughroute_skill->move( duration_of_movement , current_speed , destination_coor , graph_type );

    QSharedPointer<GWSGeometry> agent_geom_post = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    GWSCoordinate agent_position_post = agent_geom_post->getCentroid();

    // Set NEXTS behaviour
    if ( agent_position_post.getDistance( destination_coor ) < GWSLengthUnit( 1 ) ){
        return this->getProperty( NEXTS_IF_ARRIVED ).toArray();
    }

    if ( agent_position_post != destination_coor ){
        return this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray();
    }

}
