#include "EmptyWasteBehaviour.h"

#include "../../app/App.h"

EmptyWasteBehaviour::EmptyWasteBehaviour() : GWSBehaviour(){
}

bool EmptyWasteBehaviour::finished(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    if ( agent->getProperty( "waste_amount" ) == 0. ){
        return true;
    }
    else{
        return false;
    }

}

bool EmptyWasteBehaviour::behave(){


    QSharedPointer<GWSAgent> agent = this->getAgent();

    agent->setProperty("waste_amount", 0.);

    emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );

    return true;
}
