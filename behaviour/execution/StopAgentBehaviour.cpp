#include "StopAgentBehaviour.h"

#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

StopAgentBehaviour::StopAgentBehaviour() : GWSBehaviour(){
}



/**********************************************************************
 SLOTS
**********************************************************************/

QStringList StopAgentBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSExecutionEnvironment* env = GWSExecutionEnvironment::globalInstance();
    //agent->setProperty( "color" , "Black" );
    env->unregisterAgent( agent );

    QJsonObject json = agent->serialize();
    json.insert( GWSPhysicalEnvironment::GEOMETRY_PROP , false );

    emit GWSApp::globalInstance()->sendAgentToSocketSignal( json );

    return QStringList();
}
