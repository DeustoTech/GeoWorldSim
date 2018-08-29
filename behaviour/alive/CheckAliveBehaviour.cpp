#include "CheckAliveBehaviour.h"

/*  This behaviour aims at checking whether a GWSAgent has energy > 1 */

#include "../../app/App.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

CheckAliveBehaviour::CheckAliveBehaviour() : GWSBehaviour(){
}

bool CheckAliveBehaviour::finished(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    if ( agent->getProperty("energy").toDouble() > 0. )
        {
        //qDebug() << this->getAgent()->getProperty("@id").toString() << " you are alive, keep going!";
        //qDebug() << this->getAgent()->getProperty("energy").toString() << " energy!";
        return true;
        }
    else
        {
        qInfo() << "MUST DIE" << agent->getId();
        return false;
        }

}

bool CheckAliveBehaviour::behave(){
    QSharedPointer<GWSAgent> agent = this->getAgent();

    agent->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
    agent->setProperty( GWSAgent::ALIVE_PROP , false );
    emit GWSApp::globalInstance()->pushAgentSignal( agent->serialize() );

    GWSExecutionEnvironment::globalInstance()->unregisterAgent( agent );
    return true;
}
