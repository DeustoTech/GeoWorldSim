#include "FindClosestBehaviour.h"

#include "../../app/App.h"
#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../skill/view/ViewSkill.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"


FindClosestBehaviour::FindClosestBehaviour() : GWSBehaviour(){
}


bool FindClosestBehaviour::continueToNext(){

    return false;
}

bool FindClosestBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();
    QSharedPointer<GWSAgent> ClosestContainer = env->getNearestAgent( agent_coord , "ContainerAgent" );

    agent->setProperty( "compare_agent_id" , ClosestContainer->getId() );
    agent->color = QColor("red");

    double ClosestContainer_coordX = GWSPhysicalEnvironment::globalInstance()->getGeometry( ClosestContainer )->getCentroid().getX();
    double ClosestContainer_coordY = GWSPhysicalEnvironment::globalInstance()->getGeometry( ClosestContainer )->getCentroid().getY();

    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , ClosestContainer_coordX );
    mv->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , ClosestContainer_coordY );

    return true;

}
