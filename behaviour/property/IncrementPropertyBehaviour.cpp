#include "IncrementPropertyBehaviour.h"

#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString IncrementPropertyBehaviour::AGENT_ID = "agent_to_increment_id";
QString IncrementPropertyBehaviour::PROPERTY_NAME_PROP = "property";
QString IncrementPropertyBehaviour::INCREMENT_VALUE_PROP = "increment";
QString IncrementPropertyBehaviour::MAX_THRESHOLD_VALUE_PROP = "max_threshold";
QString IncrementPropertyBehaviour::MIN_THRESHOLD_VALUE_PROP = "min_threshold";
QString IncrementPropertyBehaviour::MAX_LIMIT_VALUE_PROP = "max_limit";
QString IncrementPropertyBehaviour::MIN_LIMIT_VALUE_PROP = "min_limit";
QString IncrementPropertyBehaviour::NEXTS_IF_MAX_THRESHOLD_REACHED = "nexts_if_max_reached";
QString IncrementPropertyBehaviour::NEXTS_IF_MIN_THRESHOLD_REACHED = "nexts_if_min_reached";
QString IncrementPropertyBehaviour::NEXTS_ELSE = "nexts_else";


IncrementPropertyBehaviour::IncrementPropertyBehaviour() : GWSBehaviour(){

}

/**********************************************************************
 SLOTS
**********************************************************************/


QPair< double , QJsonArray > IncrementPropertyBehaviour::behave(){

    QString agent_id = this->getProperty( AGENT_ID ).toString();

    QSharedPointer<GWSAgent> agent = GWSAgentEnvironment::globalInstance()->getByUID( agent_id );
    QString property_name = this->getProperty( PROPERTY_NAME_PROP ).toString();

    double value = agent->getProperty( property_name ).toDouble();
    double incremented = value + this->getProperty( INCREMENT_VALUE_PROP ).toDouble();

    QJsonValue max_limit = this->getProperty( MAX_LIMIT_VALUE_PROP );
    if( max_limit.isDouble() ){ incremented = qMin( max_limit.toDouble() , incremented ); }

    QJsonValue min_limit = this->getProperty( MIN_LIMIT_VALUE_PROP );
    if( min_limit.isDouble() ){ incremented = qMax( min_limit.toDouble() , incremented ); }

    agent->setProperty( property_name , incremented );

    QJsonValue max_threshold = this->getProperty( MAX_THRESHOLD_VALUE_PROP );
    if ( max_threshold.isDouble() && incremented >= max_threshold.toDouble() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_MAX_THRESHOLD_REACHED ).toArray() );
    }

    QJsonValue min_threshold = this->getProperty( MIN_THRESHOLD_VALUE_PROP );
    if ( min_threshold.isDouble() && incremented <= min_threshold.toDouble() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_MIN_THRESHOLD_REACHED ).toArray() );
    }

    // Else
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_ELSE ).toArray() );

}
