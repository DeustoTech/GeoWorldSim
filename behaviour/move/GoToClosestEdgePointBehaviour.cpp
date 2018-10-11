#include "GoToClosestEdgePointBehaviour.h"

#include "../../app/App.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

GoToClosestEdgePointBehaviour::GoToClosestEdgePointBehaviour() : GWSBehaviour(){
}


bool GoToClosestEdgePointBehaviour::canContinueToNext(){

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

    if ( ( agent->getProperty( "closest_edge_pointX" ) != destination_coor.getX() ) ||  ( agent->getProperty( "closest_edge_pointY" ) != destination_coor.getY() ) ){
        return false;
    }

    return true;
}


bool GoToClosestEdgePointBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    double hall_coordX = agent->getProperty( "closest_edge_pointX" ).toDouble();
    double hall_coordY = agent->getProperty( "closest_edge_pointY" ).toDouble();

    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , hall_coordX );
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , hall_coordY );

    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );

    return true;
}
