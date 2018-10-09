#include "MoveThroughRouteBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

MoveThroughRouteBehaviour::MoveThroughRouteBehaviour() : GWSBehaviour(){
}

MoveThroughRouteBehaviour::~MoveThroughRouteBehaviour(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool MoveThroughRouteBehaviour::continueToNext(){
    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();

    // No moveThroughRoute skill
    if( !mv ){
        qWarning() << QString("Agent %1 %2 wants to move but has no MoveSkill").arg( agent->staticMetaObject.className() ).arg( agent->getId() );
        return false;
    }

    // No destination for MoveThroughRouteSkill
    QString x = mv->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP ).toString();
    QString y = mv->getProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP ).toString();

    // No destination for MoveThroughRouteSkill
    if( x.isNull() || y.isNull() ){
        return false;
    }

    GWSCoordinate current_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    if ( current_position == mv->getRouteDestination() ){
        // Return true because we have really reached
        return true;
    }

    return false;
}

/**********************************************************************
 METHODS
**********************************************************************/

bool MoveThroughRouteBehaviour::behave(){
    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent can move
    QSharedPointer<MoveThroughRouteSkill> move_throughroute_skill = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    if( !move_throughroute_skill ){
        qWarning() << QString("Agent %1 does not have a move_throughroute_skill").arg( agent->getId() );
        return false;
    }

    GWSCoordinate destination_coor = move_throughroute_skill->getRouteDestination();
    if( !destination_coor.isValid() ){
        return true;
    }

    // Calculate speed
    GWSLengthUnit distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid().getDistance( destination_coor );
    if( move_throughroute_skill->getCurrentSpeed() == 0.0 ){
        move_throughroute_skill->changeSpeed( 1 );
    }

    // Move towards
    move_throughroute_skill->move( duration_of_movement );
    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );
    return true;
}
