#include "GoToHallBehaviour.h"

#include "../../app/App.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

GoToHallBehaviour::GoToHallBehaviour() : GWSBehaviour(){
}


bool GoToHallBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();

    if( !mv ){
        qInfo() << QString("Agent %1 %2 wants to move but has no MoveSkill").arg( agent->metaObject()->className() ).arg( agent->getId() );
        return true;
    }

    GWSCoordinate destination_coor = mv->getRouteDestination();
    if( !destination_coor.isValid() ){
        return false;
    }

    if ( ( agent->getProperty( "closest_edge_point" ) != destination_coor  ) ){
        return false;
    }

    return true;
}


bool GoToHallBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate hall_coord = agent->getProperty( "closest_edge_point" );

    double hall_coordX = hall_coord.getX();
    double hall_coordY = hall_coord.getY();

    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , hall_coordX );
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , hall_coordY );

    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );

    return true;
}
