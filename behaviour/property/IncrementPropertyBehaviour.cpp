#include "IncrementPropertyBehaviour.h"

QString IncrementPropertyBehaviour::PROPERTY_NAME_PROP = "property";
QString IncrementPropertyBehaviour::INCREMENT_VALUE_PROP = "increment";
QString IncrementPropertyBehaviour::MAX_VALUE_PROP = "max";

IncrementPropertyBehaviour::IncrementPropertyBehaviour(GWSAgent *behaving_agent) : GWSBehaviour( behaving_agent ){

}

/**********************************************************************
 SLOTS
**********************************************************************/

bool IncrementPropertyBehaviour::finished(){
    GWSAgent* agent = this->getAgent();
    QVariant value = agent->getProperty( this->getProperty( PROPERTY_NAME_PROP ).toString() );
    QVariant max_value = this->getProperty( MAX_VALUE_PROP );
    return value == max_value;
}

bool IncrementPropertyBehaviour::behave(){

    GWSAgent* agent = this->getAgent();
    QString property_name = this->getProperty( PROPERTY_NAME_PROP ).toString();
    QVariant value = agent->getProperty( property_name );
    QVariant max_value = this->getProperty( MAX_VALUE_PROP );
    QVariant incremented = value.toLongLong() + this->getProperty( INCREMENT_VALUE_PROP ).toLongLong();
    if( max_value.isValid() ){ incremented = qMin( max_value , incremented ); }
    this->getAgent()->setProperty( property_name , incremented );

    return true;
}