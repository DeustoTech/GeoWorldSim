#include "SetHomeBehaviour.h"

#include "../../app/App.h"

#include "../../environment/physical_environment/PhysicalEnvironment.h"

SetHomeBehaviour::SetHomeBehaviour() : GWSBehaviour(){

}


bool SetHomeBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
   // this->next_behaviours.clear();

    if ( agent->getProperty( "home_coordX" ).toDouble() == 0. || agent->getProperty( "home_coordY" ).toDouble() == 0. ){
        qWarning() << QString("No home coordinates for agent %1 %2 ").arg( agent->metaObject()->className() ).arg( agent->getId() );
        return false;
    }

    else{
        qWarning() << QString("Home coordinates set agent %1 %2 ").arg( agent->metaObject()->className() ).arg( agent->getId() );
        //this->next_behaviours.append( agent->getBehaviour( "BH0" ) );
        return true;
    }

}


bool SetHomeBehaviour::behave(){

    // Store initial HumanAgent coordinates as its home so that one can afterwards access them there.
    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSCoordinate agent_coord = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();

    double home_coordX = agent_coord.getX();
    double home_coordY = agent_coord.getY();

    agent->setProperty( "home_coordX" , home_coordX );
    agent->setProperty( "home_coordY" , home_coordY );


    return true;

}
