#include "IncrementPropertyBehaviour.h"

#include "../../app/App.h"

QString IncrementPropertyBehaviour::PROPERTY_NAME_PROP = "property";
QString IncrementPropertyBehaviour::INCREMENT_VALUE_PROP = "increment";
QString IncrementPropertyBehaviour::MAX_VALUE_PROP = "max";
QString IncrementPropertyBehaviour::MIN_VALUE_PROP = "min";

IncrementPropertyBehaviour::IncrementPropertyBehaviour() : GWSBehaviour(){

}

/**********************************************************************
 SLOTS
**********************************************************************/

bool IncrementPropertyBehaviour::continueToNext(){
    QSharedPointer<GWSAgent> agent = this->getAgent();
    QVariant value = agent->getProperty( this->getProperty( PROPERTY_NAME_PROP ).toString() );
    QVariant max_value = this->getProperty( MAX_VALUE_PROP );
    QVariant min_value = this->getProperty( MIN_VALUE_PROP );
    return value >= max_value || value <= min_value;
}

bool IncrementPropertyBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QString property_name = this->getProperty( PROPERTY_NAME_PROP ).toString();
    QVariant value = agent->getProperty( property_name );
    QVariant max_value = this->getProperty( MAX_VALUE_PROP );
    QVariant min_value = this->getProperty( MIN_VALUE_PROP );
    QVariant incremented = value.toDouble() + this->getProperty( INCREMENT_VALUE_PROP ).toDouble();
    if( max_value.isValid() ){ incremented = qMin( max_value , incremented ); }
    this->getAgent()->setProperty( property_name , incremented );
    emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );
    return true;
}
