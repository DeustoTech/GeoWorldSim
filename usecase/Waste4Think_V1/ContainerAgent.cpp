#include "ContainerAgent.h"

#include "../../app/App.h"

ContainerAgent::ContainerAgent() : GWSAgent(){
}


void ContainerAgent::behave(){

    double capacity = this->getProperty("capacity").toDouble();
    this->opacity = capacity / 50;

    //qDebug() << "I'm alive and my capacity is " << capacity ;

    //emit GWSApp::globalInstance()->pushAgentSignal( this->getAgent()->serialize() );
}

