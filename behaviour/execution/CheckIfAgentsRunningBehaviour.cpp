#include "CheckIfAgentsRunningBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

QString CheckIfAgentsRunningBehaviour::AGENTS_TYPE = "agents_type";
QString CheckIfAgentsRunningBehaviour::AGENTS_FILTER = "agents_filter";
QString CheckIfAgentsRunningBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString CheckIfAgentsRunningBehaviour::NEXTS_IF_TRUE = "nexts_if_true";

CheckIfAgentsRunningBehaviour::CheckIfAgentsRunningBehaviour() : GWSBehaviour(){
}

/**********************************************************************
 SLOTS
**********************************************************************/

QPair< double , QJsonArray > CheckIfAgentsRunningBehaviour::behave(){

    QString agents_type = this->getProperty( AGENTS_TYPE ).toString();
    QJsonObject agents_filter = this->getProperty( AGENTS_FILTER ).toObject();

    QList< QSharedPointer<GWSAgent> > agents = GWSExecutionEnvironment::globalInstance()->getRunningAgentsByClass<GWSAgent>( agents_type );

    foreach( QSharedPointer<GWSAgent> a , agents ) {
        if( !a->fulfillsFilter( agents_filter ) ){
            continue;
        }

        // Check if running
        quint64 current_datetime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
        quint64 agent_datetime = a->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble();
        if( agent_datetime > 0 && current_datetime <= agent_datetime && a->getProperty( GWSExecutionEnvironment::AGENT_RUNNING_PROP ).toBool() ){

            // Return true
            return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_TRUE ).toArray() );
        }
    }

    // Return false
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_FALSE ).toArray() );
}
