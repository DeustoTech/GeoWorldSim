#include "EmptyWasteBehaviour.h"

#include "../../app/App.h"

EmptyWasteBehaviour::EmptyWasteBehaviour() : GWSBehaviour(){
}

bool EmptyWasteBehaviour::finished(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    //this->next_behaviours.clear();

    if ( agent->getProperty( "waste_amount" ) == 0. ){
        //this->next_behaviours.append( agent->getBehaviour( "BH3" ) );
        return true;
    }

    else{
        return false;
    }

}

bool EmptyWasteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    qWarning() << "You have reached your maximum waste generation capacity. Empty!" << agent->getProperty( "waste_amount" ).toDouble();
    agent->setProperty("waste_amount", 0.);
    agent->icon_url = "https://image.flaticon.com/icons/svg/145/145852.svg";
    qDebug() << agent->getProperty("waste_amount");
    emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );
    return true;
}
