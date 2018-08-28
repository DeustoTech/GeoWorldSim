#include "SelectDestinationBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../skill/move/MoveSkill.h"

/*  This behaviour aims at randomly selecting the target destination of the GWSAgent */

SelectDestinationBehaviour::SelectDestinationBehaviour( GWSAgent* behaving_agent ) : GWSBehaviour( behaving_agent ){
}


bool SelectDestinationBehaviour::finished(){

    MoveSkill* mv = dynamic_cast<MoveSkill*>(this->getAgent()->getSkill( MoveSkill::staticMetaObject.className() ) );
    if( !mv ){
        qInfo() << QString("Agent %1 %2 wants to move but has no MoveSkill").arg( this->getAgent()->metaObject()->className() ).arg( this->getAgent()->getId() );
        return true;
    }

    GWSCoordinate destination_coor = mv->getDestination();
    if( !destination_coor.isValid() ){
        return false;
    }

    GWSCoordinate agent_coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent()->getId() )->getCentroid();
    return agent_coor != destination_coor;
}

bool SelectDestinationBehaviour::behave(){

    /* Move randomly through random index generator */
    int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
    int RandIndexX = qrand() % 3; //generates a random number between 0 and 2
    int RandIndexY = qrand() % 3; //generates a random number between 0 and 2

    /* Move coordinates according to random index */
    int TargetX = direction[RandIndexX];
    int TargetY = direction[RandIndexY];

    int DestinationX = TargetX + GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent()->getId() )->getCentroid().getX();
    int DestinationY = TargetY + GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent()->getId() )->getCentroid().getY();

    MoveSkill* mv = dynamic_cast<MoveSkill*>(this->getAgent()->getSkill( MoveSkill::staticMetaObject.className() ) );
    mv->setProperty( MoveSkill::DESTINATION_X_PROP , DestinationX );
    mv->setProperty( MoveSkill::DESTINATION_Y_PROP , DestinationY );

    qDebug() << "Agent = ", this->getAgent()->getProperty("@id").toString();
    qDebug() << "Destination = ", mv->getDestination().toString();
    qDebug() << "Position = " << GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent()->getId() )->getCentroid().toString();
    return true;
}
