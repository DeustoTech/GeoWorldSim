#include "MoveBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveSkill.h"

QString MoveBehaviour::AGENT_MAX_SPEED = "maxspeed";
QString MoveBehaviour::AGENT_CURRENT_SPEED = "current_speed";
QString MoveBehaviour::AGENT_MOVE_TO_X_VALUE = "move_to_x_value";
QString MoveBehaviour::AGENT_MOVE_TO_Y_VALUE = "move_to_y_value";
QString MoveBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString MoveBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";

MoveBehaviour::MoveBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QJsonArray MoveBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<MoveSkill> move_skill = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
    if( move_skill.isNull() ){
        move_skill = QSharedPointer<MoveSkill>( new MoveSkill() );
        agent->addSkill( move_skill );
    }

    GWSGeometry agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );

    QJsonValue x_destination = this->getProperty( AGENT_MOVE_TO_X_VALUE );
    QJsonValue y_destination = this->getProperty( AGENT_MOVE_TO_Y_VALUE );
    GWSCoordinate destination_coor = GWSCoordinate( x_destination.toDouble() , y_destination.toDouble() );
    if( !destination_coor.isValid() ){
        return this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray();
    }

    // Calculate speed
    GWSSpeedUnit current_speed = GWSSpeedUnit( this->getProperty( AGENT_CURRENT_SPEED ).toDouble() );
    GWSSpeedUnit max_speed = GWSSpeedUnit( this->getProperty( AGENT_MAX_SPEED ).toDouble() );

    if( current_speed == 0 ){
        current_speed = move_skill->calculateNewSpeed( current_speed , max_speed , 1.4 );
        this->setProperty( AGENT_CURRENT_SPEED , current_speed.number() );
    }

    // Move towards
    GWSCoordinate agent_position = agent_geom.getCentroid();
    move_skill->move( duration_of_movement , current_speed , destination_coor );

    if ( agent_position == destination_coor ){
        return this->getProperty( NEXTS_IF_ARRIVED ).toArray();
    }

    if ( agent_position != destination_coor ){
        return this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray();
    }

}
