#include "CheckGeneratedWasteAmountBehaviour.h"
#include "../../app/App.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

/*  This behaviour aims at checking whether a human has reached its maximum waste generation level */

DecideAccordingToWasteBehaviour::DecideAccordingToWasteBehaviour() : GWSBehaviour(){
}


bool DecideAccordingToWasteBehaviour::finished(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    this->next_behaviours.clear();


    if ( agent->property("waste_amount").toDouble() < 100 )
        {
        qDebug() << agent->property("waste_amount").toDouble();
        qInfo() << "Keep generating waste!";
        this->next_behaviours.append( agent->getBehaviour( "BH1" ) );
        return true;
        }
    else
        {
        qDebug() << agent->property("waste_amount").toDouble();
        qInfo() << "Maximum waste generated! Find container!";
        this->next_behaviours.append( agent->getBehaviour( "BH2" ) );
        return true;
        }

}

bool DecideAccordingToWasteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    agent->icon_url = "https://image.flaticon.com/icons/svg/1059/1059235.svg";
    emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );
    return true;
}

