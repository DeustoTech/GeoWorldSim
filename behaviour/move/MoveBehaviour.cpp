#include "MoveBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveSkill.h"

MoveBehaviour::MoveBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool MoveBehaviour::finished(){
    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<MoveSkill> mv = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
    // No move skill
    if( !mv ){
        qWarning() << QString("Agent %1 %2 wants to move but has no MoveSkill").arg( agent->staticMetaObject.className() ).arg( agent->getId() );
        return true;
    }
    if( mv->getProperty( MoveSkill::DESTINATION_X_PROP ).isNull() || mv->getProperty( MoveSkill::DESTINATION_Y_PROP ).isNull() ){
        return true;
    }
    return GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid() == GWSCoordinate( mv->getProperty( MoveSkill::DESTINATION_X_PROP ).toDouble() , mv->getProperty( MoveSkill::DESTINATION_Y_PROP ).toDouble() );
}

/**********************************************************************
 METHODS
**********************************************************************/

bool MoveBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    qDebug() << "Initial position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid().toString();
    qDebug() << "Moving" << this->getAgent()->getProperty("@id");

    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent can move
    QSharedPointer<MoveSkill> move_skill = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
    if( !move_skill ){
        qWarning() << QString("Agent %1 does not have a MoveSkill").arg( agent->getId() );
        return false;
    }

    // Calculate speed
    GWSCoordinate destination_coor = move_skill->getDestination();
    qDebug() << "destination_coor.toString()" << destination_coor.toString();
    GWSLengthUnit distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid().getDistance( destination_coor );
    if( move_skill->getCurrentSpeed() == 0.0 ){
        move_skill->changeSpeed( 1 );
    }

    // Move towards
    move_skill->move( duration_of_movement );
    emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );

    return true;
}
