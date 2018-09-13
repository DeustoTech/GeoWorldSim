#include "CheckIfPropertyBehaviour.h"

QString CheckIfPropertyBehaviour::CHECK_PROPERTY_NAME_PROP = "property_name";
QString CheckIfPropertyBehaviour::CHECK_PROPERTY_VALUE_PROP = "check_value";

CheckIfPropertyBehaviour::CheckIfPropertyBehaviour() : GWSBehaviour(){
}

bool CheckIfPropertyBehaviour::continueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QVariant compare_value = this->getProperty( CHECK_PROPERTY_VALUE_PROP );
    QVariant agent_property = agent->getProperty( this->getProperty( CHECK_PROPERTY_NAME_PROP ).toString() );

    if( agent_property == compare_value ){
        return true;
    }

    return false;
}

bool CheckIfPropertyBehaviour::behave(){
    // EMPTY, DO NOTHING
}
