#include "GoHomeBehaviour.h"

#include "../../behaviour/Behaviour.h"
#include "../../app/App.h"
#include "../../skill/move/MoveSkill.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"

GoHomeBehaviour::GoHomeBehaviour() : GWSBehaviour(){

}

bool GoHomeBehaviour::finished(){

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

bool GoHomeBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    double home_coordX = agent->getProperty("home_coordX").toDouble();
    double home_coordY = agent->getProperty("home_coordY").toDouble();

    QSharedPointer<MoveSkill> mv = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
    mv->setProperty( MoveSkill::DESTINATION_X_PROP , home_coordX );
    mv->setProperty( MoveSkill::DESTINATION_Y_PROP , home_coordY );

    emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );

    return true;
}
