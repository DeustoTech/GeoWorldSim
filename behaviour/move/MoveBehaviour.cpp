#include "MoveBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveSkill.h"

QString MoveBehaviour::SET_MAX_SPEED = "maxspeed";
QString MoveBehaviour::SET_MOVE_TO_X_VALUE = "move_to_x_value";
QString MoveBehaviour::SET_MOVE_TO_Y_VALUE = "move_to_y_value";
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

    // Set max speed if any
    if( !this->getProperty( SET_MAX_SPEED ).isNull() ){
        agent->setProperty( MoveSkill::AGENT_MAX_SPEED_PROP , this->getProperty( SET_MAX_SPEED ) );
    }

    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );

    QJsonValue x_destination = this->getProperty( SET_MOVE_TO_X_VALUE );
    QJsonValue y_destination = this->getProperty( SET_MOVE_TO_Y_VALUE );
    agent->setProperty( MoveSkill::AGENT_MOVING_TOWARDS_X_PROP , x_destination );
    agent->setProperty( MoveSkill::AGENT_MOVING_TOWARDS_Y_PROP , y_destination );

    GWSCoordinate destination_coor = move_skill->getMovingTowardsCoordinate();
    if( !destination_coor.isValid() ){
        return this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray();
    }

    // Calculate speed
    GWSCoordinate agent_position = agent_geom->getCentroid();
    GWSLengthUnit distance = agent_position.getDistance( destination_coor );
    if( move_skill->getCurrentSpeed() == 0.0 ){
        move_skill->changeSpeed( 1 );
    }

    // Move towards
    move_skill->move( duration_of_movement , 4 , destination_coor );

    if ( agent_position == destination_coor ){
        return this->getProperty( NEXTS_IF_ARRIVED ).toArray();
    }

    if ( agent_position != destination_coor ){
        return this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray();
    }

}
