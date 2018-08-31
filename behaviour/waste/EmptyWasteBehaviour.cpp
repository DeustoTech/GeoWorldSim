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
    agent->icon_url = "https://image.flaticon.com/icons/svg/145/145852.svg";

   emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );
   return true;
}
