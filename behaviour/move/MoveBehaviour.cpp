#include "MoveBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"
#include "../../skill/move/MoveSkill.h"

QString geoworldsim::behaviour::MoveBehaviour::INPUT_MOVE_TO = "input_move_to";
QString geoworldsim::behaviour::MoveBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString geoworldsim::behaviour::MoveBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";

geoworldsim::behaviour::MoveBehaviour::MoveBehaviour() : Behaviour(){
}

void geoworldsim::behaviour::MoveBehaviour::afterCreateHook() {

    QSharedPointer<Entity> entity = this->getEntity();

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer< skill::MoveSkill > move_skill = entity->getSkill( skill::MoveSkill::staticMetaObject.className() ).dynamicCast< skill::MoveSkill >();
    if( move_skill.isNull() ){
        move_skill = QSharedPointer< skill::MoveSkill >( new skill::MoveSkill() );
        entity->addSkill( move_skill );
    }

}

/**********************************************************************
 METHODS
**********************************************************************/

QPair<double, QJsonArray> geoworldsim::behaviour::MoveBehaviour::behave(){

    QSharedPointer< Entity > entity = this->getEntity();

    // Tick in 1 second duration to move in small parts
    unit::TimeUnit duration_of_movement = this->getProperty( BEHAVIOUR_DURATION ).toDouble();

    geometry::Geometry entity_geom = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    QSharedPointer< skill::MoveSkill > move_skill = entity->getSkill( skill::MoveSkill::staticMetaObject.className() ).dynamicCast< skill::MoveSkill >();

    geometry::Geometry destination = this->getProperty( INPUT_MOVE_TO ).toObject();
    if( !destination.isValid() ){
        qWarning() << QString("Agent %1 %2 has no destination to move to.").arg( entity->metaObject()->className() ).arg( entity->getUID() );
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
    }

    // Calculate speed
    unit::SpeedUnit current_speed = unit::SpeedUnit( entity->getProperty( skill::MoveSkill::INSTANT_SPEED ).toDouble() );
    unit::SpeedUnit max_speed = unit::SpeedUnit( entity->getProperty( skill::MoveSkill::MAX_SPEED ).toDouble( 1.4 ) );

    if( current_speed == 0 ){
        current_speed = move_skill->calculateNewSpeed( current_speed , max_speed , 1.4 );
        entity->setProperty( skill::MoveSkill::INSTANT_SPEED , current_speed.number() );
    }

    // Pending time to reach destination can be higher than the duration requested.
    unit::LengthUnit pending_distance = entity_geom.getDistance( destination );
    unit::TimeUnit pending_time = pending_distance.number() / current_speed.number(); // Time needed to reach route_destination at current speed
    duration_of_movement = qMin( pending_time , duration_of_movement );

    // Move towards
    move_skill->move( duration_of_movement , current_speed , destination );

    geometry::Geometry entity_geom_post = geometry::Geometry( entity->getProperty( geoworldsim::environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    if ( entity_geom_post.getDistance( destination ) < unit::LengthUnit( 0.5 ) ){
        return QPair< double , QJsonArray >( duration_of_movement.number() , this->getProperty( NEXTS_IF_ARRIVED ).toArray() );
    }

    return QPair< double , QJsonArray >( duration_of_movement.number() , this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
}
