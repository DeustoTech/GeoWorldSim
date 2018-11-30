#include "IncrementPropertyBehaviour.h"

#include "../../app/App.h"

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

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QString property_name = this->getProperty( PROPERTY_NAME_PROP ).toString();
    QVariant value = agent->getProperty( property_name );
    QVariant max_value = this->getProperty( MAX_VALUE_PROP );
    QVariant min_value = this->getProperty( MIN_VALUE_PROP );
    QVariant incremented = value.toDouble() + this->getProperty( INCREMENT_VALUE_PROP ).toDouble();
    if( max_value.isValid() ){ incremented = qMin( max_value , incremented ); }

    agent->setProperty( property_name , incremented.toJsonValue() );

    if ( incremented <= min_value ){
        return this->getProperty( NEXTS_IF_MIN ).toArray();
    }

    if ( incremented >= max_value ){
        return this->getProperty( NEXTS_IF_MAX ).toArray();
    }

    // Else
    return this->getProperty( NEXTS_IF_WITHIN_THRESHOLD ).toArray();

}
