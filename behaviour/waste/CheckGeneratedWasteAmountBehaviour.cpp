#include "CheckGeneratedWasteAmountBehaviour.h"
#include "../../app/App.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

/*  This behaviour aims at checking whether a human has reached its maximum waste generation level */

CheckGeneratedWasteAmountBehaviour::CheckGeneratedWasteAmountBehaviour() : GWSBehaviour(){
}


bool CheckGeneratedWasteAmountBehaviour::finished(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    if ( agent->property("waste_amount").toDouble() < 100 )
        {
        qDebug() << agent->property("waste_amount").toDouble();
        qInfo() << "Keep generating waste!";
        return true;
        }
    else
        {
        qDebug() << agent->property("waste_amount").toDouble();
        qInfo() << "Maximum waste generated! Find container!";
        return false;
        }

}

bool CheckGeneratedWasteAmountBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    agent->icon_url = "https://image.flaticon.com/icons/svg/1059/1059235.svg";
    emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );
    return true;
}

