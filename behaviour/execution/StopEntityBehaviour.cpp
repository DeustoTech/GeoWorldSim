#include "StopEntityBehaviour.h"

#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

StopEntityBehaviour::StopEntityBehaviour() : GWSBehaviour(){
}


/**********************************************************************
 SLOTS
**********************************************************************/

QPair<double, QJsonArray> StopEntityBehaviour::behave(){

    QSharedPointer<GWSEntity> entity = this->getEntity();
    GWSExecutionEnvironment* env = GWSExecutionEnvironment::globalInstance();
    env->unregisterEntity( entity );

    QJsonObject json = entity->serialize();
    emit GWSCommunicationEnvironment::globalInstance()->sendAgentSignal( json );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , QJsonArray({ this->getUID() }) );
}
