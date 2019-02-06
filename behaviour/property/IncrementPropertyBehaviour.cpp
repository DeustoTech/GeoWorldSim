#include "IncrementPropertyBehaviour.h"

#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString IncrementPropertyBehaviour::AGENT_ID = "agent_to_increment_id";
QString IncrementPropertyBehaviour::PROPERTY_NAME_PROP = "property";
QString IncrementPropertyBehaviour::INCREMENT_VALUE_PROP = "increment";
QString IncrementPropertyBehaviour::MAX_VALUE_PROP = "max";
QString IncrementPropertyBehaviour::MIN_VALUE_PROP = "min";
QString IncrementPropertyBehaviour::NEXTS_IF_MAX = "nexts_if_max";
QString IncrementPropertyBehaviour::NEXTS_IF_MIN = "nexts_if_min";
QString IncrementPropertyBehaviour::NEXTS_IF_WITHIN_THRESHOLD = "nexts_if_within_threshold";



IncrementPropertyBehaviour::IncrementPropertyBehaviour() : GWSBehaviour(){

}

/**********************************************************************
 SLOTS
**********************************************************************/


QJsonArray IncrementPropertyBehaviour::behave(){

    QString agent_id = this->getProperty( AGENT_ID ).toString();

    QSharedPointer<GWSAgent> agent = GWSAgentEnvironment::globalInstance()->getById( agent_id );
    QString property_name = this->getProperty( PROPERTY_NAME_PROP ).toString();
    double value = agent->getProperty( property_name ).toDouble();
    QJsonValue max_value = this->getProperty( MAX_VALUE_PROP );
    QJsonValue min_value = this->getProperty( MIN_VALUE_PROP );
    double incremented = value + this->getProperty( INCREMENT_VALUE_PROP ).toDouble();
    if( max_value.isDouble() ){ incremented = qMin( max_value.toDouble() , incremented ); }
    if( min_value.isDouble() ){ incremented = qMax( min_value.toDouble() , incremented ); }

    agent->setProperty( property_name , incremented );

    if ( max_value.isDouble() && incremented <= min_value.toDouble() ){
        return this->getProperty( NEXTS_IF_MIN ).toArray();
    }

    if ( min_value.isDouble() && incremented >= max_value.toDouble() ){
        return this->getProperty( NEXTS_IF_MAX ).toArray();
    }

    // Else
    return this->getProperty( NEXTS_IF_WITHIN_THRESHOLD ).toArray();

}
