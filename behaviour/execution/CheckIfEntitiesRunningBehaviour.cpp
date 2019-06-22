#include "CheckIfEntitiesRunningBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

QString CheckIfEntitiesRunningBehaviour::ENTITY_TYPE = "entity_type";
QString CheckIfEntitiesRunningBehaviour::ENTITY_FILTER = "entity_filter";
QString CheckIfEntitiesRunningBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString CheckIfEntitiesRunningBehaviour::NEXTS_IF_TRUE = "nexts_if_true";

CheckIfEntitiesRunningBehaviour::CheckIfEntitiesRunningBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 SLOTS
**********************************************************************/

QPair<double, QJsonArray> CheckIfEntitiesRunningBehaviour::behave(){

    QString entity_type = this->getProperty( ENTITY_TYPE ).toString();
    QJsonObject entity_filter = this->getProperty( ENTITY_FILTER ).toObject();

    QList< QSharedPointer<GWSEntity> > entities = GWSExecutionEnvironment::globalInstance()->getRunningByClass<GWSEntity>( entity_type );

    foreach( QSharedPointer<GWSEntity> a , entities ) {
        if( !a->fulfillsFilter( entity_filter ) ){
            continue;
        }

        // Check if running
        quint64 current_datetime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
        quint64 agent_datetime = a->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble();
        if( agent_datetime > 0 && current_datetime <= agent_datetime && a->getProperty( GWSExecutionEnvironment::ENTITY_RUNNING_PROP ).toBool() ){

            // Return true
            return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_TRUE ).toArray() );
        }
    }

    // Return false
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
}
