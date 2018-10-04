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

   /* QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<MoveThroughRouteSkill> mv = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    GWSCoordinate current_position = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid();


    if ( current_position == mv->getRouteDestination() ){
        return false;
    }
    return true;*/

    /*QSharedPointer<GWSAgent> agent = this->getAgent();

    if ( agent->getProperty( "waste_amount" ) != 0. ){
        return false;
    }
    return true;*/
    return false;
}

/**********************************************************************
 METHODS
**********************************************************************/


bool ExchangePropertyBehaviour::behave(){

    qDebug() << "Exchange Property behaviour";

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
    other_agent->setProperty( "waste_amount" , agent->getProperty( "waste_amount" ) );

    // And that changes the value of the property of the agent:
    agent->setProperty("waste_amount", 0.);
    agent->color = QColor("green");

    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );
    emit GWSApp::globalInstance()->sendAgentSignal( other_agent->serialize() );

    return true;

}
