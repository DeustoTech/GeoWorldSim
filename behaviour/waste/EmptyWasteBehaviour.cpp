#include "EmptyWasteBehaviour.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../app/App.h"

EmptyWasteBehaviour::EmptyWasteBehaviour() : GWSBehaviour(){
}

bool EmptyWasteBehaviour::canContinueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    if ( agent->getProperty( "waste_amount" ) != 0. ){
        return false;
    }
    return true;

}

QStringList EmptyWasteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString other_agent_id = agent->getProperty( "compare_agent_id" ).toString();
    QSharedPointer< GWSAgent > other_agent = GWSAgentEnvironment::globalInstance()->getByClassAndId( GWSAgent::staticMetaObject.className() , other_agent_id );

    // The other agent takes the property of the agent:
    other_agent->setProperty( "waste_amount" , agent->getProperty( "waste_amount" ) );

    agent->setProperty("waste_amount", 0.);

    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );
    emit GWSApp::globalInstance()->sendAgentSignal( other_agent->serialize() );
    return true;
}
