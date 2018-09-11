#include "EmptyWasteBehaviour.h"

#include "../../app/App.h"

EmptyWasteBehaviour::EmptyWasteBehaviour() : GWSBehaviour(){
}

bool EmptyWasteBehaviour::finished(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    //this->next_behaviours.clear();

    if ( agent->getProperty( "waste_amount" ) != 0. ){
        return false;
    }
    return true;

    /*if ( agent->getProperty( "waste_amount" ) == 0. ){
        //this->next_behaviours.append( agent->getBehaviour( "BH5" ) );
        return true;}*/
}

bool EmptyWasteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    agent->setProperty("waste_amount", 0.);

    emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );

    return true;
}
