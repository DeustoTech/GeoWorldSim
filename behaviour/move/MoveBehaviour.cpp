#include "MoveBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveSkill.h"

QString MoveBehaviour::MAX_SPEED_PROP = "maxspeed";
QString MoveBehaviour::X_VALUE = "x_value";
QString MoveBehaviour::Y_VALUE = "y_value";
QString MoveBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString MoveBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";

MoveBehaviour::MoveBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QStringList MoveBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coors = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();


    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<MoveSkill> move_skill = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
    if( move_skill.isNull() ){
        move_skill = QSharedPointer<MoveSkill>( new MoveSkill() );
        agent->addSkill( move_skill );
    }
    move_skill->setProperty( MoveSkill::MAX_SPEED_PROP , this->getProperty( MoveBehaviour::MAX_SPEED_PROP ) );

    QVariant x_destination = this->getProperty( X_VALUE );
    QVariant y_destination = this->getProperty( Y_VALUE );

    bool x_is_property = x_destination.toString().startsWith( "<" ) && x_destination.toString().endsWith( ">" );
    bool y_is_property = y_destination.toString().startsWith( "<" ) && y_destination.toString().endsWith( ">" );

    if ( x_is_property && y_is_property ){

        QString x_property_name = x_destination.toString().remove( 0 , 1 );
        QString y_property_name = y_destination.toString().remove( 0 , 1 );

        x_property_name = x_property_name.remove( x_property_name.length() - 1 , 1 );
        y_property_name = y_property_name.remove( y_property_name.length() - 1 , 1 );

        x_destination = agent->getProperty( x_property_name );
        y_destination = agent->getProperty( y_property_name );

    }

    move_skill->setProperty( MoveSkill::DESTINATION_X_PROP , x_destination.toDouble() );
    move_skill->setProperty( MoveSkill::DESTINATION_Y_PROP , y_destination.toDouble() );

    GWSCoordinate destination_coor = move_skill->getCurrentDestination();
    if( !destination_coor.isValid() ){
        QStringList nexts = this->getProperty( NEXTS_IF_NOT_ARRIVED ).toStringList();
    }

    // Calculate speed
    GWSLengthUnit distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid().getDistance( destination_coor );
    if( move_skill->getCurrentSpeed() == 0.0 ){
        move_skill->changeSpeed( 1 );
    }

    // Move towards
    move_skill->move( duration_of_movement );

    GWSCoordinate agent_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    if ( agent_position == destination_coor ){
        QStringList nexts = this->getProperty( NEXTS_IF_ARRIVED ).toStringList();
        return nexts;
    }

    if ( agent_position != destination_coor ){
        QStringList nexts = this->getProperty( NEXTS_IF_NOT_ARRIVED ).toStringList();
        return nexts;
    }

}
