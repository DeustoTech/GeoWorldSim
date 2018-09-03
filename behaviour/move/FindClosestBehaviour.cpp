#include "FindClosestBehaviour.h"

#include "../../app/App.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/view/ViewSkill.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"


FindClosestBehaviour::FindClosestBehaviour() : GWSBehaviour(){
}


bool FindClosestBehaviour::finished(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<MoveSkill> mv = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
    if( !mv ){
        qInfo() << QString("Agent %1 %2 wants to move but has no MoveSkill").arg( agent->metaObject()->className() ).arg( agent->getId() );
        return true;
    }

    GWSCoordinate destination_coor = mv->getDestination();
    if( !destination_coor.isValid() ){
        return false;
    }

    GWSCoordinate agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
    return agent_coor != destination_coor;
}

bool FindClosestBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();
    GWSPhysicalEnvironment* env = GWSPhysicalEnvironment::globalInstance();
    QSharedPointer<GWSAgent> ClosestContainerAgent = env->getNearestAgent( agent_coord , "ContainerAgent");

    double ClosestContainerAgent_coordX = GWSPhysicalEnvironment::globalInstance()->getGeometry( ClosestContainerAgent )->getCentroid().getX();
    double ClosestContainerAgent_coordY = GWSPhysicalEnvironment::globalInstance()->getGeometry( ClosestContainerAgent )->getCentroid().getY();


    QSharedPointer<MoveSkill> mv = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
    mv->setProperty( MoveSkill::DESTINATION_X_PROP , ClosestContainerAgent_coordX );
    mv->setProperty( MoveSkill::DESTINATION_Y_PROP , ClosestContainerAgent_coordY );

    qDebug() << "Found closest container at " << ClosestContainerAgent_coordX << "," << ClosestContainerAgent_coordY;
    ClosestContainerAgent->icon_url = "https://image.flaticon.com/icons/svg/820/820741.svg";

    emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );

    return true;

}
