#include "WolfAgent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"


WolfAgent::WolfAgent(QObject *parent) : GWSAgent( parent ) {
    qDebug() << "WOLF";
}


void SheepAgent::behave()
{
        qDebug() << "---------------- Wolfie ---------------------";

        // Send information to website
        emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );

}
