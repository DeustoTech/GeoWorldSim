#include "FindClosestBehaviour.h"


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

    /*QList< QSharedPointer<GWSAgent> > neighbouringAgents = agent->getSkill( ViewSkill::staticMetaObject.className() ).dynamicCast<ViewSkill>()->getViewingAgents();

    for (int i = 0; i < neighbouringAgents.size(); i++)
        {
        if (neighbouringAgents.at(i)->getProperty("@type").toString() == "ContainerAgent"){
            // extract closest
            int DestinationX = 0;
            int DestinationY = 0;
        }
    }


    QSharedPointer<MoveSkill> mv = agent->getSkill( MoveSkill::staticMetaObject.className() ).dynamicCast<MoveSkill>();
    mv->setProperty( MoveSkill::DESTINATION_X_PROP , DestinationX );
    mv->setProperty( MoveSkill::DESTINATION_Y_PROP , DestinationY );*/

}
