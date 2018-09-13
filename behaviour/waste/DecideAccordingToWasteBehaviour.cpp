#include "DecideAccordingToWasteBehaviour.h"
#include "../../app/App.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

/*  This behaviour aims at checking whether a human has reached its maximum waste generation level */

DecideAccordingToWasteBehaviour::DecideAccordingToWasteBehaviour() : GWSBehaviour(){
}


bool DecideAccordingToWasteBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    if ( agent->property("waste_amount").toDouble() < 100. )
        {
        qDebug() << agent->property("waste_amount").toDouble();
        qInfo() << "Check if at home position in order to generate waste!";
        return false;
        }
    else
        {
        qDebug() << agent->property("waste_amount").toDouble();
        qInfo() << "Maximum waste generated! Find container!";
        return true;
        }

}

bool DecideAccordingToWasteBehaviour::behave(){
    return true;
}

