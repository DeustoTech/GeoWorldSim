#include "StopEntityBehaviour.h"

#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

geoworldsim::behaviour::StopEntityBehaviour::StopEntityBehaviour() : Behaviour(){
}


/**********************************************************************
 SLOTS
**********************************************************************/

QPair<double, QJsonArray> geoworldsim::behaviour::StopEntityBehaviour::behave(){

    QSharedPointer< Entity > entity = this->getEntity();
    environment::ExecutionEnvironment* env = environment::ExecutionEnvironment::globalInstance();
    env->unregisterEntity( entity );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , QJsonArray({ this->getUID() }) );
}
