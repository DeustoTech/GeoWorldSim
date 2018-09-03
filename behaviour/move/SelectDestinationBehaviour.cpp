#include "SelectDestinationBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../skill/move/MoveSkill.h"

/*  This behaviour aims at randomly selecting the target destination of the GWSAgent */

SelectDestinationBehaviour::SelectDestinationBehaviour() : GWSBehaviour(){
}

bool SelectDestinationBehaviour::finished(){

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

bool SelectDestinationBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    /* Move randomly through random index generator */
    int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
    int RandIndexX = qrand() % 3; //generates a random number between 0 and 2
    int RandIndexY = qrand() % 3; //generates a random number between 0 and 2

    /* Move coordinates according to random index */
    int TargetX = direction[RandIndexX];
    int TargetY = direction[RandIndexY];

    qDebug() << "Target" << TargetX;
    qDebug() << TargetY;

    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    if( !agent_geom ){
        qWarning() << QString("Agent %1 %2 tried to select destination without geometry").arg( agent->metaObject()->className() ).arg( agent->getId() );
    }

    qDebug() << "Agent centroid" << agent_geom->getCentroid().getX();
    qDebug() << agent_geom->getCentroid().getY();

    int DestinationX = TargetX + agent_geom->getCentroid().getX();
    int DestinationY = TargetY + agent_geom->getCentroid().getY();

    qDebug() << "Destination" << DestinationX;
    qDebug() << DestinationY;

    QSharedPointer<MoveSkill> mv = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
    mv->setProperty( MoveSkill::DESTINATION_X_PROP , DestinationX );
    mv->setProperty( MoveSkill::DESTINATION_Y_PROP , DestinationY );

    //qDebug() << "Agent = ", agent->getId();
    qDebug() << "Destination = ", mv->getDestination();


    return true;
}
