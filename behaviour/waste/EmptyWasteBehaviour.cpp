#include "EmptyWasteBehaviour.h"

#include "../../app/App.h"

EmptyWasteBehaviour::EmptyWasteBehaviour() : GWSBehaviour(){
}

bool EmptyWasteBehaviour::finished(){
    return false;
}

bool EmptyWasteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    qDebug() << "Empty waste" ;
    agent->setProperty("waste_amount", 0.);




   emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );
   return true;
}
