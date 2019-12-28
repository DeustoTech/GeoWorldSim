#include "GoHomeBehaviour.h"

#include "../../behaviour/Behaviour.h"
#include "../../app/App.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"

GoHomeBehaviour::GoHomeBehaviour() : GWSBehaviour(){

}

bool GoHomeBehaviour::canContinueToNext(){

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

    if ( ( agent->getProperty("home_coordX") != destination_coor.getX() ) || ( agent->getProperty( "home_coordY" ) != destination_coor.getY() ) ){
        return false;
    }

    return true;
}

QStringList GoHomeBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    double home_coordX = agent->getProperty("home_coordX").toDouble();
    double home_coordY = agent->getProperty("home_coordY").toDouble();

    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , home_coordX );
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , home_coordY );

    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );

    return true;
}
