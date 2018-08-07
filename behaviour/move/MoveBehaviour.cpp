#include "MoveBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../agent/Agent.h"
#include "../../skill/move/MoveSkill.h"

QString MoveBehaviour::DESTINATION_X_PROP = "destination_x";
QString MoveBehaviour::DESTINATION_Y_PROP = "destination_y";

MoveBehaviour::MoveBehaviour( GWSAgent* behaving_agent ) : GWSBehaviour( behaving_agent ){
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool MoveBehaviour::finished(){
    if( this->getProperty( DESTINATION_X_PROP ).isNull() || this->getProperty( DESTINATION_Y_PROP ).isNull() ){
        return true;
    }
    return GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent() )->getCentroid() == GWSCoordinate( this->getProperty( DESTINATION_X_PROP ).toDouble() , this->getProperty( DESTINATION_Y_PROP ).toDouble() );
}

/**********************************************************************
 METHODS
**********************************************************************/

bool MoveBehaviour::behave(){

    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent can move
    MoveSkill* move_skill = dynamic_cast<MoveSkill*>( this->getAgent()->getSkill( MoveSkill::staticMetaObject.className() ) );
    if( !move_skill ){
        qWarning() << QString("Agent %1 does not have a MoveSkill").arg( this->getAgent()->getId() );
        return false;
    }

    // Calculate speed
    GWSCoordinate destination_coor = GWSCoordinate( this->getProperty( DESTINATION_X_PROP ).toDouble() , this->getProperty( DESTINATION_Y_PROP ).toDouble() );
    GWSLengthUnit distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent() )->getCentroid().getDistance( destination_coor );
    if( move_skill->getCurrentSpeed() == 0.0 ){
        move_skill->changeSpeed( 1 );
    }

    // Move towards
    move_skill->moveTowards( destination_coor , duration_of_movement );

    return true;
}
