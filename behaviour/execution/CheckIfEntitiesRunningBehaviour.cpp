#include "CheckIfEntitiesRunningBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

QString geoworldsim::behaviour::CheckIfEntitiesRunningBehaviour::ENTITY_TYPE = "entity_type";
QString geoworldsim::behaviour::CheckIfEntitiesRunningBehaviour::ENTITY_FILTER = "entity_filter";
QString geoworldsim::behaviour::CheckIfEntitiesRunningBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString geoworldsim::behaviour::CheckIfEntitiesRunningBehaviour::NEXTS_IF_TRUE = "nexts_if_true";

geoworldsim::behaviour::CheckIfEntitiesRunningBehaviour::CheckIfEntitiesRunningBehaviour() : Behaviour(){
}

/**********************************************************************
 SLOTS
**********************************************************************/

QPair<double, QJsonArray> geoworldsim::behaviour::CheckIfEntitiesRunningBehaviour::behave(){

    QString entity_type = this->getProperty( ENTITY_TYPE ).toString();
    QJsonObject entity_filter = this->getProperty( ENTITY_FILTER ).toObject();

    QList< QSharedPointer<Entity> > entities = environment::ExecutionEnvironment::globalInstance()->getRunning<Entity>( entity_type );

    foreach( QSharedPointer<Entity> a , entities ) {
        if( !a->fulfillsFilter( entity_filter ) ){
            continue;
        }

        // Check if running
        quint64 current_datetime = environment::TimeEnvironment::globalInstance()->getCurrentDateTime();
        quint64 agent_datetime = a->getProperty( environment::Environment::INTERNAL_TIME_PROP ).toDouble();
        if( agent_datetime > 0 && current_datetime <= agent_datetime && a->getProperty( environment::ExecutionEnvironment::ENTITY_RUNNING_PROP ).toBool() ){

            // Return true
            return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_TRUE ).toArray() );
        }
    }

    // Return false
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
}
