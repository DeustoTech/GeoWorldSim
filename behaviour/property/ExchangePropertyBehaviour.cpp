#include "ExchangePropertyBehaviour.h"


#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

ExchangePropertyBehaviour::ExchangePropertyBehaviour() : GWSBehaviour(){
}

/*
 *  This behaviour will be executed each time the truck reaches a container,
 *  so that the truck can gather the waste of the container, and then continue
 *  its route to the next container (i.e. pass through the LoopOverRouteStages
 *  behaviour)
 *
 *  When a human empties its waste into a container, agent = HumanAgent (the one that is
 *  emptied) and other = ContainerAgent (the one that is filled).
 *
 *  When a truck picks the waste from a container up, it is the other way round:
 *  agent = TruckAgent (the one that is filled up) and other = ContainerAgent (emptied)
 */


/**********************************************************************
 GETTERS
**********************************************************************/

bool ExchangePropertyBehaviour::continueToNext(){

    QSharedPointer< GWSAgent > agent = this->getAgent();
    QString other_agent_id = agent->getProperty( "compare_agent_id" ).toString();
    QSharedPointer< GWSAgent > other_agent =  GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , other_agent_id );

    if ( ( agent->getProperty( "@type" ) == "HumanAgent" )  && ( agent->getProperty( "waste_amount" ) != 0. ) ){
        return false;

    }

    if ( ( agent->getProperty( "@type" ) == "TruckAgent" ) && ( other_agent->getProperty( "waste_amount" ) != 0. ) ){
        return false;
    }



    return true;
}

/**********************************************************************
 METHODS
**********************************************************************/


bool ExchangePropertyBehaviour::behave(){

    QSharedPointer< GWSAgent > agent;
    QSharedPointer< GWSAgent > other_agent;
    QString other_agent_id;
    QString agent_id;

    if ( this->getAgent()->getProperty( "@type" ) == "HumanAgent" ){

        agent = this->getAgent();
        other_agent_id = agent->getProperty( "compare_agent_id" ).toString();
        other_agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , other_agent_id );

    }

    if ( this->getAgent()->getProperty( "@type" ) == "TruckAgent" ){

        other_agent = this->getAgent();
        agent_id = other_agent->getProperty( "compare_agent_id" ).toString();
        agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , agent_id );


    }


    // The other agent takes the property of the agent:
    double new_waste = other_agent->getProperty( "waste_amount" ).toDouble() + agent->getProperty( "waste_amount" ).toDouble();
    other_agent->setProperty( "waste_amount" , new_waste );

    // And that changes the value of the property of the agent:
    agent->setProperty("waste_amount", 0.);
    agent->color = QColor("green");

    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );
    emit GWSApp::globalInstance()->sendAgentSignal( other_agent->serialize() );

    return true;

}
