#include "MoveBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../agent/Agent.h"
#include "../../skill/move/MoveSkill.h"

MoveBehaviour::MoveBehaviour( GWSAgent* behaving_agent ) : GWSBehaviour( behaving_agent ){
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool MoveBehaviour::finished(){
    MoveSkill* mv = dynamic_cast<MoveSkill*>(this->getAgent()->getSkill( MoveSkill::staticMetaObject.className() ) );
    // No move skill
    if( !mv ){
        qWarning() << QString("Agent %1 %2 wants to move but has no MoveSkill").arg( this->getAgent()->staticMetaObject.className() ).arg( this->getId() );
        return true;
    }
    if( mv->getProperty( MoveSkill::DESTINATION_X_PROP ).isNull() || mv->getProperty( MoveSkill::DESTINATION_Y_PROP ).isNull() ){
        return true;
    }
    return GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent() )->getCentroid() == GWSCoordinate( mv->getProperty( MoveSkill::DESTINATION_X_PROP ).toDouble() , mv->getProperty( MoveSkill::DESTINATION_Y_PROP ).toDouble() );
}

/**********************************************************************
 METHODS
**********************************************************************/

bool MoveBehaviour::behave(){

    qDebug() << "Initial position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry(this->getAgent() )->getCentroid().toString();
    qDebug() << "Moving" << this->getAgent()->getProperty("@id");

    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent can move
    MoveSkill* move_skill = dynamic_cast<MoveSkill*>( this->getAgent()->getSkill( MoveSkill::staticMetaObject.className() ) );
    if( !move_skill ){
        qWarning() << QString("Agent %1 does not have a MoveSkill").arg( this->getAgent()->getId() );
        return false;
    }

    // Calculate speed
    GWSCoordinate destination_coor = move_skill->getDestination();
    qDebug() << "destination_coor.toString()" << destination_coor.toString();
    GWSLengthUnit distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent() )->getCentroid().getDistance( destination_coor );
    if( move_skill->getCurrentSpeed() == 0.0 ){
        move_skill->changeSpeed( 1 );
    }

    // Move towards
    move_skill->move( duration_of_movement );

    return true;
}
