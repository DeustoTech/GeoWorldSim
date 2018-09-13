#include "EmptyWasteBehaviour.h"

#include "../../app/App.h"

EmptyWasteBehaviour::EmptyWasteBehaviour() : GWSBehaviour(){
}

bool EmptyWasteBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    if ( agent->getProperty( "waste_amount" ) != 0. ){
        return false;
    }
    return true;

}

bool EmptyWasteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    agent->setProperty("waste_amount", 0.);

    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );

    return true;
}
